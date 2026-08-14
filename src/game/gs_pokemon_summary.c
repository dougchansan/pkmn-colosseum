/**
 * @file gs_pokemon_summary.c
 * @brief GSpokeSummary -- Pokemon summary screen and status display.
 *
 * Address range: 0x80015000 - 0x800181C4 (~30 functions)
 *
 * This module implements the Pokemon summary/status screen that displays
 * detailed information about a single Pokemon. It handles:
 *   - Multi-page summary display (Info, Moves, Stats, Ribbons, etc.)
 *   - Move detail popup with type/power/accuracy
 *   - Contest stat visualization
 *   - Ribbon collection display
 *   - Shadow Pokemon purification gauge
 *
 * The summary screen operates as a sub-state of the party menu. When
 * the player selects "Summary" on a party Pokemon, this module takes
 * over rendering and input handling.
 *
 * Key functions:
 *   fn_8001501C  GSpokeSummary_DrawLabel      -- 0x34 bytes, render text label
 *   fn_80015050  GSpokeSummary_DrawHandler     -- 0x94 bytes, invoke page draw handler
 *   fn_800150E4  GSpokeSummary_DrawPage        -- 0x290 bytes, main page renderer
 *   fn_80015374  GSpokeSummary_DrawMovePage     -- 0x23C bytes, move list page
 *   fn_800155B0  GSpokeSummary_DrawStatsPage    -- 0x40C bytes, stat hexagon page
 *   fn_800159BC  GSpokeSummary_DrawRibbonPage   -- 0x480 bytes, ribbon collection
 *   fn_80015E3C  GSpokeSummary_ProcessInput     -- 0x374 bytes, input handler
 *   fn_800161B0  GSpokeSummary_PageTransition   -- 0x198 bytes, page flip animation
 *   fn_80016348  GSpokeSummary_UpdateCursor     -- 0x188 bytes, cursor movement
 *   fn_800164D0  GSpokeSummary_MoveDetail       -- 0x148 bytes, move detail popup
 *   fn_80016618  GSpokeSummary_GetPageCount     -- 0xA4 bytes, count available pages
 *   fn_800166BC  GSpokeSummary_SetupPage        -- 0x114 bytes, initialize page data
 *   fn_800167D0  GSpokeSummary_DrawTypeIcon     -- 0x2EC bytes, draw Pokemon type icon
 *   fn_80016ABC  GSpokeSummary_DrawStatBar      -- 0x458 bytes, draw stat bar graphic
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
extern void* heroItemGetItemKindToItemAryPtr();
extern void* heroHizukiItemGetItemAryPtr();
extern u8    fn_801429E8(void* fieldData);    /* Check field validity */
extern u16   itemBiosGetNum(void* fieldData);    /* Get field value */
extern u16   itemDataBiosGetPtr(u16 speciesId);      /* Get species data */
extern u16   itemDataBiosGetPrice(void);               /* Get display field count */

/* Text rendering */
extern void  msgctrlSetValue(s32 paramId, s32 value);
extern u32   GSmsgGetRect(s32 resourceId);
extern void  fn_800FB680(s32 x, s32 y, s32 flags, u32 color);

/* Math/rendering helpers */
extern f32   sin(f32);
extern f32   cos(f32);
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

/* fn_8001501C - 0x8001501C | size: 0x34 */
extern u32 lbl_8047A2DC;
#if 0
asm void fn_8001501C(void) {
#include "src/game/gs_pokemon_summary_fn_8001501C.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
s32 fn_8001501C(void) {
    fn_800FB680(0, 0, -1, lbl_8047A2DC);
    return 0;
}
#pragma pop
#endif

/* fn_80015050 - 0x80015050 | size: 0x94 */
extern u32 cursorBiosGetPos(u32 a);
extern u8 lbl_80266918[];
#define sSummaryPageEntries lbl_80266918
typedef struct SummaryPageEntry {
    u8 displayColor[3];      /* 0x00, copied to output bytes 0x64-0x66 */
    u8 unk_03;
    s32 dataSource;          /* 0x04, -1 uses default party source */
    u8 unk_08[4];
    s32 colorMatchId;        /* 0x0C, used by fn_80017790 */
    s32 gaugeMatchId;        /* 0x10, compared with output species/id */
    u8 unk_14[4];
    void* drawHandler;       /* 0x18 */
    u32 messageId;           /* 0x1C */
    u8 unk_20[0x2C];
} SummaryPageEntry;
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
#define SUMMARY_PAGE_DISPLAY_COLOR(index, component) \
    (sSummaryPageEntries[(index) * sizeof(SummaryPageEntry) + (component)])

typedef s32 (*SummaryPageDrawFn)(s32 x, s32 pageIndex, u16* messageParam);
typedef s32 (*DrawHandlerFn)(u8*, u8*, u16*);
typedef s32 (*SummaryMenuCallback)(s32, s32, s32*);

extern u32 itemBiosGetItemDataId(void*);
extern u32 itemDataBiosGetName(void);
extern u32 itemDataBiosGetKinomiNo(u16);
extern u32 itemDataBiosGetWazaMachineNo(void);
extern u32 itemDataBiosGetHidenMachineNo(u32);
extern u32 itemDataBiosGetWazaIDByWazaMachineNo(u32);
extern u32 itemDataBiosGetDoc(void);
extern u32 fn_80143F9C(void);
extern u32 itemDataBiosGetBattleUseFunc(void);
extern u32 wazaDataBiosGetPtr(u16);
extern u32 wazaDataBiosGetName(u32);
extern u8* windowGetKeyInfo(void);
extern void cursorBiosSetPos(u32, u16*);
extern void fn_800FE38C(s32, s32, s32, s32);
extern void fn_800FE35C(void);
extern void fn_800FE6D0(s32, s32);
extern void spriteSetEnv(void);
extern void windowDrawSprite2(s32, s32, s32, s32, u32, s32, s32, s32);
extern void winSeqSetMenu(s32, s32);
extern void fn_80166A50(s32, s32, s32, s32);
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
#if 0
asm void fn_80015050(void) {
#include "src/game/gs_pokemon_summary_fn_80015050.inc"
}
#else
#pragma push
#pragma peephole off
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
#pragma pop
#endif

/* fn_800150E4 - 0x800150E4 | size: 0x290 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
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
    u16 count;
    u16 itemCount;
    u8* entry;
    void* list;
    void* field;
    s32 dataSource;
    s32 validCount;
    s32 listIndex;
    s32 visibleIndex;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleLimit;
    s32 scrollPixels;
    s32 y;
    s32 textX;
    u32 value;
    u32 textId;

    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    field = list;
    for (listIndex = 0; listIndex < count; listIndex++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    itemIndex = (s32)(s8)((u8*)packedRange)[0];
    displayLine = 0;
    visibleLimit = 8;
    scrollPixels = 0;
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            visibleLimit = 9;
        }
        scrollPixels = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    visibleIndex = displayLine;
    while (visibleIndex < visibleLimit && itemIndex < validCount) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scrollPixels;
            textX = x;
            if (itemIndex + 1 < 10) {
                msgctrlSetValue(0x34, 0);
                textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);
            }
            msgctrlSetValue(0x34, itemIndex + 1);
            fn_800FB680(textX, y, -1, 0xCA);
            textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);

            dataSource = SUMMARY_ENTRY_FIELD(entry);
            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }

            value = 0;
            listIndex = -1;
            while (++listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    if (--itemIndex < 0) {
                        value = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }

            itemDataBiosGetPtr((u16)value);
            textId = itemDataBiosGetName();
            fn_800FB680(textX + 0xA, y, -1, textId);
            itemIndex += listIndex + 1;
        }
        displayLine++;
        visibleIndex++;
        itemIndex++;
    }

    if (visibleIndex < 8) {
        fn_800FB680(x, visibleIndex * 0x1F - scrollPixels, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_80015374 - 0x80015374 | size: 0x23c */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_80015374(void) {
#include "src/game/gs_pokemon_summary_fn_80015374.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80015374(s32 x, s32 pageIndex, u16* packedRange) {
    u16 count;
    u16 itemCount;
    u8* entry;
    void* list;
    void* field;
    s32 dataSource;
    s32 validCount;
    s32 listIndex;
    s32 visibleIndex;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleLimit;
    s32 scrollPixels;
    s32 y;
    u32 value;
    u32 textId;

    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    field = list;
    for (listIndex = 0; listIndex < count; listIndex++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    itemIndex = (s32)(s8)((u8*)packedRange)[0];
    displayLine = 0;
    visibleLimit = 8;
    scrollPixels = 0;
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            visibleLimit = 9;
        }
        scrollPixels = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    visibleIndex = displayLine;
    while (visibleIndex < visibleLimit && itemIndex < validCount) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scrollPixels;
            dataSource = SUMMARY_ENTRY_FIELD(entry);
            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }

            value = 0;
            listIndex = -1;
            while (++listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    if (--itemIndex < 0) {
                        value = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }

            itemDataBiosGetPtr((u16)value);
            textId = itemDataBiosGetName();
            if (textId != 0) {
                fn_800FB680(x, y, -1, textId);
            }
            itemIndex += listIndex + 1;
        }
        displayLine++;
        visibleIndex++;
        itemIndex++;
    }

    if (visibleIndex < visibleLimit) {
        fn_800FB680(x, visibleIndex * 0x1F - scrollPixels, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_800155B0 - 0x800155B0 | size: 0x40c */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
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
    u16 count;
    u16 itemCount;
    u16 totalCount;
    u8* entry;
    void* list;
    void* field;
    s32 dataSource;
    s32 validCount;
    s32 listIndex;
    s32 visibleIndex;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleLimit;
    s32 scrollPixels;
    s32 y;
    s32 textX;
    u32 species;
    u32 value;
    u32 textId;
    u32 level;

    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    field = list;
    for (listIndex = 0; listIndex < count; listIndex++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    itemIndex = (s32)(s8)((u8*)packedRange)[0];
    displayLine = 0;
    visibleLimit = 8;
    scrollPixels = 0;
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            visibleLimit = 9;
        }
        scrollPixels = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    visibleIndex = displayLine;
    while (visibleIndex < visibleLimit && itemIndex < validCount) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scrollPixels;
            fn_800FB680(x, y, -1, 0x2AFF);
            textX = x + (s32)(u16)(GSmsgGetRect(0x2AFF) >> 16);

            dataSource = SUMMARY_ENTRY_FIELD(entry);
            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }

            species = 0;
            listIndex = -1;
            while (++listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    if (--itemIndex < 0) {
                        species = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }

            level = (u8)(itemDataBiosGetKinomiNo((u16)species) + 1);
            if (level < 10) {
                msgctrlSetValue(0x34, 0);
                fn_800FB680(textX, y, -1, 0xCA);
                textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);
            }
            msgctrlSetValue(0x34, level);
            fn_800FB680(textX, y, -1, 0xCA);
            textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);
            itemDataBiosGetPtr(species);
            textId = itemDataBiosGetName();
            fn_800FB680(textX + 9, y, -1, textId);

            dataSource = SUMMARY_ENTRY_FIELD(entry);
            field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &totalCount, 0, 0, 0);
            msgctrlSetValue(0x34, totalCount);
            textX = x + 0x11A - ((s16)(GSmsgGetRect(0xCA) >> 16) + (s16)(GSmsgGetRect(0x12E) >> 16));
            fn_800FB680(textX, y, -1, 0x12E);

            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }
            value = 0;
            listIndex = -1;
            while (++listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    if (--itemIndex < 0) {
                        value = itemBiosGetNum(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }
            msgctrlSetValue(0x34, (u16)value);
            fn_800FB680(x + 0x11A - (s16)(GSmsgGetRect(0xCA) >> 16), y, -1, 0xCA);
            itemIndex += listIndex + 1;
        }
        displayLine++;
        visibleIndex++;
        itemIndex++;
    }

    if (visibleIndex < 8) {
        fn_800FB680(x, visibleIndex * 0x1F - scrollPixels, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_800159BC - 0x800159BC | size: 0x480 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
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
    u16 count;
    u16 itemCount;
    u16 totalCount;
    u8* entry;
    s32* dataSourcePtr;
    void* list;
    void* field;
    s32 dataSource;
    s32 validCount;
    s32 validItemIndex;
    s32 listIndex;
    s32 visibleIndex;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleLimit;
    s32 scrollPixels;
    s32 y;
    s32 textX;
    u32 species;
    u32 speciesData;
    u32 rank;
    u32 displayRank;
    u32 subRank;
    u8 subDisplayRank;
    u32 messageId;
    u32 value;

    displayLine = 0;
    visibleLimit = 8;
    scrollPixels = 0;
    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    field = list;
    for (listIndex = 0; listIndex < count; listIndex++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    itemIndex = (s32)(s8)((u8*)packedRange)[0];
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            visibleLimit = 9;
        }
        scrollPixels = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    dataSourcePtr = (s32*)(SUMMARY_ENTRY_RAW(pageIndex) + 4);
    visibleIndex = displayLine;
    while (visibleIndex < visibleLimit && itemIndex < validCount) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scrollPixels;
            dataSource = *dataSourcePtr;
            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }
            species = 0;
            listIndex = 0;
            validItemIndex = -1;
            while (listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    validItemIndex++;
                    if (validItemIndex >= itemIndex) {
                        species = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
                listIndex++;
            }

            speciesData = itemDataBiosGetPtr((u16)species);
            rank = (u8)itemDataBiosGetWazaMachineNo();
            displayRank = rank + 1;
            subRank = itemDataBiosGetHidenMachineNo(speciesData);
            subDisplayRank = (u8)(subRank + 1);
            messageId = ((subRank & 0xff) != 0xFF) ? 0x2B00 : 0x2AFF;

            fn_800FB680(x, y, -1, messageId);
            textX = x + (s32)(u16)(GSmsgGetRect(messageId) >> 16);
            if ((subRank & 0xff) != 0xFF) {
                msgctrlSetValue(0x34, subDisplayRank);
                fn_800FB680(textX, y, -1, 0xCA);
                textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);
            } else {
                if ((s32)displayRank < 10) {
                    msgctrlSetValue(0x34, 0);
                    fn_800FB680(textX, y, -1, 0xCA);
                    textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);
                }
                msgctrlSetValue(0x34, displayRank);
                fn_800FB680(textX, y, -1, 0xCA);
                textX += (s32)(u16)(GSmsgGetRect(0xCA) >> 16);
            }

            messageId = wazaDataBiosGetName(wazaDataBiosGetPtr((u16)itemDataBiosGetWazaIDByWazaMachineNo(rank)));
            fn_800FB680(textX + 9, y, -1, messageId);

            dataSource = *dataSourcePtr;
            heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &totalCount, 0, 0, 0);
            msgctrlSetValue(0x34, totalCount);
            textX = x + 0x11A - ((s16)(GSmsgGetRect(0xCA) >> 16) + (s16)(GSmsgGetRect(0x12E) >> 16));
            fn_800FB680(textX, y, -1, 0x12E);

            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }
            value = 0;
            listIndex = 0;
            validItemIndex = -1;
            while (listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    validItemIndex++;
                    if (validItemIndex >= itemIndex) {
                        value = itemBiosGetNum(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
                listIndex++;
            }
            msgctrlSetValue(0x34, (u16)value);
            fn_800FB680(x + 0x11A - (s16)(GSmsgGetRect(0xCA) >> 16), y, -1, 0xCA);
            itemIndex += listIndex + 1;
        }
        displayLine++;
        visibleIndex++;
        itemIndex++;
    }

    if (visibleIndex < 8) {
        fn_800FB680(x, visibleIndex * 0x1F - scrollPixels, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_80015E3C - 0x80015E3C | size: 0x374 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 0
asm void fn_80015E3C(void) {
#include "src/game/gs_pokemon_summary_fn_80015E3C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80015E3C(s32 x, s32 pageIndex, u16* packedRange) {
    u16 count;
    u16 itemCount;
    u16 totalCount;
    u8* entry;
    void* list;
    void* field;
    s32 dataSource;
    s32 validCount;
    s32 listIndex;
    s32 visibleIndex;
    s32 itemIndex;
    s32 displayLine;
    s32 visibleLimit;
    s32 scrollPixels;
    s32 y;
    s32 textX;
    u32 value;
    u32 textId;

    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    field = list;
    for (listIndex = 0; listIndex < count; listIndex++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    itemIndex = (s32)(s8)((u8*)packedRange)[0];
    displayLine = 0;
    visibleLimit = 8;
    scrollPixels = 0;
    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0) && (s32)lbl_8047A2C8 != 0) {
        if (SUMMARY_F32(lbl_8047A2D0) < SUMMARY_F32(lbl_8047B748)) {
            itemIndex--;
            displayLine = -1;
        } else {
            visibleLimit = 9;
        }
        scrollPixels = (s32)SUMMARY_F32(lbl_8047A2D0);
    }

    visibleIndex = displayLine;
    while (visibleIndex < visibleLimit && itemIndex < validCount) {
        if (itemIndex >= 0) {
            y = displayLine * 0x1F - scrollPixels;
            dataSource = SUMMARY_ENTRY_FIELD(entry);
            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }
            value = 0;
            listIndex = -1;
            while (++listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    if (--itemIndex < 0) {
                        value = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }
            itemDataBiosGetPtr((u16)value);
            textId = itemDataBiosGetName();
            if (textId != 0) {
                fn_800FB680(x, y, -1, textId);
            }

            dataSource = SUMMARY_ENTRY_FIELD(entry);
            heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &totalCount, 0, 0, 0);
            msgctrlSetValue(0x34, totalCount);
            textX = x + 0x11A - ((s16)(GSmsgGetRect(0xCA) >> 16) + (s16)(GSmsgGetRect(0x12E) >> 16));
            fn_800FB680(textX, y, -1, 0x12E);

            if (dataSource >= 0) {
                field = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &itemCount, 0, 0, 0);
            } else {
                field = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &itemCount, 0, 0, 0);
            }
            value = 0;
            listIndex = -1;
            while (++listIndex < itemCount) {
                if (fn_801429E8(field) != 0) {
                    if (--itemIndex < 0) {
                        value = itemBiosGetNum(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }
            msgctrlSetValue(0x34, (u16)value);
            fn_800FB680(x + 0x11A - (s16)(GSmsgGetRect(0xCA) >> 16), y, -1, 0xCA);
            itemIndex += listIndex + 1;
        }
        displayLine++;
        visibleIndex++;
        itemIndex++;
    }

    if (visibleIndex < visibleLimit) {
        fn_800FB680(x, visibleIndex * 0x1F - scrollPixels, -1, 0x2AFE);
    }
    return 0;
}
#pragma pop
#endif

/* fn_800161B0 - 0x800161B0 | size: 0x198 */
extern u8 lbl_802EF0A8[];
extern u32 lbl_8047A2D4;
extern u32 lbl_8047B748;
extern u32 lbl_8047B750;
#if 0
asm void fn_800161B0(void) {
#include "src/game/gs_pokemon_summary_fn_800161B0.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800161B0(u8* ctx, u8* item) {
    u8* entry;
    u8* neighborEntry;
    SummaryPageDrawFn pageFn;
    s32 pageIndex;
    s32 neighborIndex;
    s32 x;
    u16 messageParam;
    f32 neighborX;

    fn_800FE38C(
        SUMMARY_ITEM_S16(lbl_802EF0A8, 0x3BBA) -
            SUMMARY_ITEM_S16(lbl_802EF0A8 + SUMMARY_ITEM_S16(item, 0x06) * 0x1C, 0x02),
        SUMMARY_ITEM_S16(lbl_802EF0A8, 0x3BBC) -
            SUMMARY_ITEM_S16(lbl_802EF0A8 + SUMMARY_ITEM_S16(item, 0x06) * 0x1C, 0x04),
        SUMMARY_ITEM_S16(lbl_802EF0A8, 0x3BBE),
        SUMMARY_ITEM_S16(lbl_802EF0A8, 0x3BC0));

    pageIndex = SUMMARY_CTX_S8(ctx, 0x95);
    entry = SUMMARY_ENTRY_RAW(pageIndex);
    messageParam = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(entry)) >> 16);
    pageFn = SUMMARY_ENTRY_PAGE_FN(entry);
    if (pageFn != NULL) {
        pageFn((s32)SUMMARY_F32(lbl_8047A2D4), pageIndex, &messageParam);
    }

    if (SUMMARY_F32(lbl_8047A2D4) != SUMMARY_F32(lbl_8047B748)) {
        if (SUMMARY_F32(lbl_8047A2D4) > SUMMARY_F32(lbl_8047B748)) {
            neighborIndex = pageIndex + 1;
            if (neighborIndex >= 6) {
                neighborIndex = 1;
            }
            neighborX = SUMMARY_F32(lbl_8047A2D4) - SUMMARY_F32(lbl_8047B750);
        } else {
            neighborIndex = pageIndex - 1;
            if (neighborIndex < 1) {
                neighborIndex = 5;
            }
            neighborX = SUMMARY_F32(lbl_8047B750) + SUMMARY_F32(lbl_8047A2D4);
        }

        neighborEntry = SUMMARY_ENTRY_RAW(neighborIndex);
        pageFn = SUMMARY_ENTRY_PAGE_FN(neighborEntry);
        if (pageFn != NULL) {
            messageParam = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(neighborEntry)) >> 16);
            x = (s32)neighborX;
            pageFn(x, neighborIndex, &messageParam);
        }
    }

    fn_800FE35C();
    return 0;
}
#pragma pop
#endif

/* fn_80016348 - 0x80016348 | size: 0x188 */
extern u32 lbl_8047A2D4;
extern u32 lbl_8047B748;
extern u32 lbl_8047B750;
#if 0
asm void fn_80016348(void) {
#include "src/game/gs_pokemon_summary_fn_80016348.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80016348(u8* ctx, u8* item) {
    u8* entry;
    s32 pageIndex;
    s32 neighborIndex;
    s32 halfItemWidth;
    s32 halfTextWidth;
    s32 textId;
    f32 neighborX;

    fn_800FE38C(0, 0, SUMMARY_ITEM_S16(item, 0x54), SUMMARY_ITEM_S16(item, 0x56));

    pageIndex = SUMMARY_CTX_S8(ctx, 0x95);
    entry = SUMMARY_ENTRY_RAW(pageIndex);
    textId = *(s32*)(entry + 0x20);
    halfItemWidth = SUMMARY_ITEM_S16(item, 0x54) / 2;
    halfTextWidth = (s16)(GSmsgGetRect(textId) >> 16) / 2;
    fn_800FB680((s32)SUMMARY_F32(lbl_8047A2D4) + halfItemWidth - halfTextWidth, 0, -1, textId);

    if (SUMMARY_F32(lbl_8047A2D4) != SUMMARY_F32(lbl_8047B748)) {
        if (SUMMARY_F32(lbl_8047A2D4) > SUMMARY_F32(lbl_8047B748)) {
            neighborIndex = pageIndex + 1;
            if (neighborIndex >= 6) {
                neighborIndex = 1;
            }
            neighborX = SUMMARY_F32(lbl_8047A2D4) - SUMMARY_F32(lbl_8047B750);
        } else {
            neighborIndex = pageIndex - 1;
            if (neighborIndex < 1) {
                neighborIndex = 5;
            }
            neighborX = SUMMARY_F32(lbl_8047B750) + SUMMARY_F32(lbl_8047A2D4);
        }

        entry = SUMMARY_ENTRY_RAW(neighborIndex);
        textId = *(s32*)(entry + 0x20);
        halfTextWidth = (s16)(GSmsgGetRect(textId) >> 16) / 2;
        fn_800FB680((s32)neighborX + halfItemWidth - halfTextWidth, 0, -1, textId);
    }

    fn_800FE35C();
    return 0;
}
#pragma pop
#endif

/* fn_800164D0 - 0x800164D0 | size: 0x148 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2D8;
extern u32 lbl_8047B744;
extern u32 lbl_8047A2C4;
extern u32 lbl_8047B740;
#if 0
asm void fn_800164D0(void) {
#include "src/game/gs_pokemon_summary_fn_800164D0.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800164D0(u8* ctx, u8* item) {
    u8* entry;
    void* list;
    void* field;
    u16 count;
    u16 packed;
    s32 threshold;
    s32 validCount;
    s32 i;
    s32 dataSource;

    entry = SUMMARY_ENTRY_RAW(SUMMARY_CTX_S8(ctx, 0x95));
    packed = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(entry)) >> 16);
    threshold = (s32)(s8)((u8*)&packed)[0] + 8;
    dataSource = SUMMARY_ENTRY_FIELD(entry);

    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    validCount = 0;
    field = list;
    for (i = 0; i < count; i++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    if (threshold < validCount + 1 && (s32)lbl_8047A2D8 == -1) {
        SUMMARY_ITEM_U8(item, 0x67) =
            SUMMARY_F32(lbl_8047B740) * (SUMMARY_F32(lbl_8047B744) - SUMMARY_F32(lbl_8047A2C4));
    } else {
        SUMMARY_ITEM_U8(item, 0x67) = 0;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80016618 - 0x80016618 | size: 0xa4 */
extern u32 lbl_8047A2D8;
extern u32 lbl_8047B744;
extern u32 lbl_8047A2C4;
extern u32 lbl_8047B740;
#if 0
asm void fn_80016618(void) {
#include "src/game/gs_pokemon_summary_fn_80016618.inc"
}
#else
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

/* fn_800166BC - 0x800166BC | size: 0x114 */
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047B768;
extern u32 lbl_8047B754;
extern u32 lbl_8047B758;
extern u32 lbl_8047B75C;
extern u32 lbl_8047B760;
#if 0
asm void fn_800166BC(void) {
#include "src/game/gs_pokemon_summary_fn_800166BC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800166BC(u8* ctx, u8* item) {
    u8* entry;
    u16 packed;
    s32 row;
    s32 column;
    s32 y;
    f32 angle;

    entry = SUMMARY_ENTRY_RAW(SUMMARY_CTX_S8(ctx, 0x95));
    packed = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(entry)) >> 16);
    row = (s32)(s8)((u8*)&packed)[1];
    column = (s32)(s8)((u8*)&packed)[0];

    y = row * 0x1F + 0x95;
    if ((s32)lbl_8047A2C8 == 0) {
        y += (s32)SUMMARY_F32(lbl_8047A2D0);
    }
    SUMMARY_ITEM_S16(item, 0x52) = (s16)y;

    angle = (f32)(((column + row) * 0x1F) + (s32)SUMMARY_F32(lbl_8047A2D0)) *
            SUMMARY_F32(lbl_8047B754);
    while (angle > SUMMARY_F32(lbl_8047B75C)) {
        angle -= SUMMARY_F32(lbl_8047B758);
    }
    while (angle < SUMMARY_F32(lbl_8047B760)) {
        angle += SUMMARY_F32(lbl_8047B758);
    }
    *(f32*)(item + 0x70) = angle;
    return 0;
}
#pragma pop
#endif

/* fn_800167D0 - 0x800167D0 | size: 0x2ec */
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern u32 lbl_8047B748;
extern u32 lbl_8047B744;
extern u32 lbl_8047B768;
extern u32 lbl_8047B75C;
extern u32 lbl_8047B770;
extern u32 lbl_8047B774;
extern u32 lbl_8047B778;
#if 0
asm void fn_800167D0(void) {
#include "src/game/gs_pokemon_summary_fn_800167D0.inc"
}
#else
#pragma push
#pragma peephole off
void fn_800167D0(u8* owner, u8* item, f32 phase, u16 iconId, u8 alpha) {
    f32 stops[5];
    f32 width;
    f32 height;
    f32 radius;
    f32 denom;
    f32 localPhase;
    f32 angle;
    s32 segment;
    s32 drawX;
    s32 drawY;

    width = (f32)SUMMARY_ITEM_S16(item, 0x54);
    height = (f32)SUMMARY_ITEM_S16(item, 0x56);
    radius = height * SUMMARY_F32(lbl_8047B75C) * SUMMARY_F32(lbl_8047B770);
    denom = SUMMARY_F32(lbl_8047B774) * (width + radius);

    stops[0] = SUMMARY_F32(lbl_8047B748);
    stops[1] = height / denom;
    stops[2] = (width + radius) / denom;
    stops[3] = (SUMMARY_F32(lbl_8047B774) * width + radius) / denom;
    stops[4] = SUMMARY_F32(lbl_8047B744);

    segment = 0;
    if (!(stops[0] <= phase && phase < stops[1])) {
        segment = 1;
        if (!(stops[1] <= phase && phase < stops[2])) {
            segment = 2;
            if (!(stops[2] <= phase && phase < stops[3])) {
                segment = 3;
                if (!(stops[3] <= phase && phase < stops[4])) {
                    segment = 4;
                }
            }
        }
    }

    localPhase = (phase - stops[segment]) / (stops[segment + 1] - stops[segment]);
    drawX = 0;
    drawY = 0;

    if (segment == 0) {
        drawX = (s32)(localPhase * width);
    }
    if (segment == 1) {
        angle = SUMMARY_F32(lbl_8047B75C) * localPhase - SUMMARY_F32(lbl_8047B778);
        drawX = (s32)(((height - SUMMARY_F32(lbl_8047B774)) * cos(angle)) *
                      SUMMARY_F32(lbl_8047B770) + width);
        drawY = (s32)((height - SUMMARY_F32(lbl_8047B774)) * sin(angle) *
                      SUMMARY_F32(lbl_8047B770) + height * SUMMARY_F32(lbl_8047B770));
    }
    if (segment == 2) {
        drawX = (s32)(height - SUMMARY_F32(lbl_8047B774));
        drawY = (s32)((SUMMARY_F32(lbl_8047B744) - localPhase) * width);
    }
    if (segment == 3) {
        angle = SUMMARY_F32(lbl_8047B75C) * localPhase + SUMMARY_F32(lbl_8047B778);
        drawX = (s32)(((height - SUMMARY_F32(lbl_8047B774)) * cos(angle)) *
                      SUMMARY_F32(lbl_8047B770));
        drawY = (s32)((height - SUMMARY_F32(lbl_8047B774)) * sin(angle) *
                      SUMMARY_F32(lbl_8047B770) + height * SUMMARY_F32(lbl_8047B770));
    }

    windowDrawSprite2(drawX, drawY, 2, 2, 0xFFFFFF00 | alpha, (s32)owner, iconId, 0);
}
#pragma pop
#endif

/* fn_80016ABC - 0x80016ABC | size: 0x458 */
extern u32 lbl_8047A2D8;
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047B768;
extern u32 lbl_8047B75C;
extern u32 lbl_8047B770;
extern u32 lbl_8047B774;
extern u32 lbl_8047A2CC;
extern u32 lbl_8047B740;
extern u32 lbl_8047B77C;
extern u32 lbl_8047B744;
extern u32 lbl_8047B780;
extern u32 lbl_8047B784;
#if 0
asm void fn_80016ABC(void) {
#include "src/game/gs_pokemon_summary_fn_80016ABC.inc"
}
#else
#pragma push
#pragma peephole off
#pragma fp_contract off
s32 fn_80016ABC(u8* ctx, u8* item) {
    u8* entry;
    u16 packed;
    s32 y;
    s32 alpha;
    s32 selected;
    f32 phase;
    f32 step;
    f32 alphaScale;
    f32 alphaDivisor;
    f32 wrap;

    if ((s32)lbl_8047A2D8 != -1) {
        return 0;
    }
    if ((s32)lbl_8047A2E8 >= 0) {
        return 0;
    }

    entry = SUMMARY_ENTRY_RAW(SUMMARY_CTX_S8(ctx, 0x95));
    packed = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(entry)) >> 16);
    selected = (s32)lbl_8047A2E8;
    if (selected >= 0) {
        y = (selected - (s32)(s8)((u8*)&packed)[0]) * 0x1F + 0x95;
        if ((s32)lbl_8047A2C8 != 0) {
            y -= (s32)SUMMARY_F32(lbl_8047A2D0);
        }
        if (y + SUMMARY_ITEM_S16(item, 0x56) < 0x95 || y >= 0x18D) {
            alpha = 0;
        } else {
            alpha = 0xFF;
        }
    } else {
        y = (s32)(s8)((u8*)&packed)[1] * 0x1F + 0x95;
        if ((s32)lbl_8047A2C8 == 0) {
            y += (s32)SUMMARY_F32(lbl_8047A2D0);
        }
        alpha = ((s32)lbl_8047A2D8 == -1) ? 0x72 : 0xFF;
    }

    SUMMARY_ITEM_S16(item, 0x52) = (s16)y;
    SUMMARY_ITEM_U8(item, 0x67) = (u8)alpha;
    fn_800FE6D0((s16)(SUMMARY_ITEM_S16(ctx, 0x84) + SUMMARY_ITEM_S16(item, 0x50)),
                (s16)(SUMMARY_ITEM_S16(ctx, 0x86) + SUMMARY_ITEM_S16(item, 0x52)));
    spriteSetEnv();

    step = SUMMARY_F32(lbl_8047B774) /
           (SUMMARY_F32(lbl_8047B774) *
            ((f32)SUMMARY_ITEM_S16(item, 0x54) +
             SUMMARY_F32(lbl_8047B75C) * (f32)SUMMARY_ITEM_S16(item, 0x56) *
                 SUMMARY_F32(lbl_8047B770)));
    alphaScale = SUMMARY_F32(lbl_8047B740);
    alphaDivisor = SUMMARY_F32(lbl_8047B77C);
    wrap = SUMMARY_F32(lbl_8047B744);
    phase = SUMMARY_F32(lbl_8047A2CC);
    for (selected = 0; selected < 0x2D; selected++) {
        fn_800167D0(ctx, item, phase, 0xD1,
                    (u8)(alphaScale * ((f32)selected / alphaDivisor)));
        phase += step;
        if (phase >= wrap) {
            phase -= wrap;
        }
    }

    phase = SUMMARY_F32(lbl_8047A2CC) + SUMMARY_F32(lbl_8047B780);
    if (phase > wrap) {
        phase -= wrap;
    }
    for (selected = 0; selected < 0x2D; selected++) {
        fn_800167D0(ctx, item, phase, 0xD1,
                    (u8)(alphaScale * ((f32)selected / alphaDivisor)));
        phase += step;
        if (phase >= wrap) {
            phase -= wrap;
        }
    }

    phase = SUMMARY_F32(lbl_8047A2CC) + SUMMARY_F32(lbl_8047B770);
    if (phase > wrap) {
        phase -= wrap;
    }
    for (selected = 0; selected < 0x2D; selected++) {
        fn_800167D0(ctx, item, phase, 0xD1,
                    (u8)(alphaScale * ((f32)selected / alphaDivisor)));
        phase += step;
        if (phase >= wrap) {
            phase -= wrap;
        }
    }

    phase = SUMMARY_F32(lbl_8047A2CC) + SUMMARY_F32(lbl_8047B784);
    if (phase > wrap) {
        phase -= wrap;
    }
    for (selected = 0; selected < 0x2D; selected++) {
        fn_800167D0(ctx, item, phase, 0xD1,
                    (u8)(alphaScale * ((f32)selected / alphaDivisor)));
        phase += step;
        if (phase >= wrap) {
            phase -= wrap;
        }
    }

    return 0;
}
#pragma pop
#endif

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
#pragma push
#pragma peephole off
s32 fn_80016F14(u8* src, u8* dst) {
    u16 tmp;
    s32 v;
    s32 col;
    tmp = (u16)(cursorBiosGetPos((u16)*(u32*)(&sSummaryPageEntries[(s32)(s8)src[0x95] * 0x4C + 0x1C])) >> 16);
    if ((s32)lbl_8047A2E8 >= 0) {
        v = ((s32)lbl_8047A2E8 - (s32)(s8)*(u8*)&tmp) * 0x1f + 0x95;
        if ((s32)lbl_8047A2C8 != 0) {
            v -= (s32)*(f32*)&lbl_8047A2D0;
        }
        if (v + (s32)*(s16*)(dst + 0x56) >= 0x95) {
            if (v >= 0x18d) {
                col = 0;
            } else {
                col = 0xff;
            }
        } else {
            col = 0;
        }
    } else {
        v = (s32)(s8)*((u8*)&tmp + 1) * 0x1f + 0x95;
        if ((s32)lbl_8047A2C8 == 0) {
            v += (s32)*(f32*)&lbl_8047A2D0;
        }
        if ((s32)lbl_8047A2D8 == -1) {
            col = 0x72;
        } else {
            col = 0xff;
        }
    }
    *(s16*)(dst + 0x52) = (s16)v;
    dst[0x67] = col;
    return 0;
}
#pragma pop
#endif

/* fn_80017028 - 0x80017028 | size: 0x73c */
extern u8 lbl_80266B88[];
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D4;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2C8;
extern u32 lbl_8047B788;
extern u32 lbl_8047B78C;
extern u32 lbl_8047B750;
extern u32 lbl_8047B790;
extern u32 lbl_8047A2DC;
#if 0
asm void fn_80017028(void) {
#include "src/game/gs_pokemon_summary_fn_80017028.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80017028(u8* ctx) {
    u8* input;
    u8* entry;
    u8* labelBase;
    u8* dataSourceBase;
    void* list;
    void* field;
    u16 packed;
    u16 count;
    u16 currentPacked;
    u32 fallbackLabels[5];
    s32 pageIndex;
    s32 dataSource;
    s32 cursorIndex;
    s32 baseIndex;
    s32 validCount;
    s32 i;
    s32 visibleIndex;
    s32 soundId;
    u32 species;
    s32 moved;

    input = windowGetKeyInfo();
    soundId = 0;
    moved = 0;
    for (i = 0; i < 5; i++) {
        fallbackLabels[i] = ((u32*)lbl_80266B88)[i];
    }

    if (SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D4) ||
        SUMMARY_F32(lbl_8047B748) != SUMMARY_F32(lbl_8047A2D0)) {
        return 0;
    }

    pageIndex = SUMMARY_CTX_S8(ctx, 0x95);
    labelBase = (u8*)sSummaryPageEntries + 0x1C;
    packed = (u16)(cursorBiosGetPos(
                       (u16)*(u32*)(labelBase + pageIndex * SUMMARY_ENTRY_STRIDE)) >>
                   16);
    dataSourceBase = (u8*)sSummaryPageEntries + 0x04;
    dataSource = *(s32*)(dataSourceBase +
                         SUMMARY_CTX_S8(ctx, 0x95) * SUMMARY_ENTRY_STRIDE);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    field = list;
    for (validCount = 0; validCount < count; validCount++) {
        fn_801429E8(field);
        field = (u8*)field + 4;
    }

    cursorIndex = (s32)(s8)((u8*)&packed)[0] + (s32)(s8)((u8*)&packed)[1];
    entry = SUMMARY_ENTRY_RAW(pageIndex);

    if ((s32)lbl_8047A2E8 < 0) {
        if ((SUMMARY_ITEM_U16(input, 0x04) & 0xC0) != 0 &&
            (s32)lbl_8047A2E0 != 3 && (s32)lbl_8047A2E0 != 4 &&
            *(s32*)(entry + 0x08) != 0) {
            field = list;
            visibleIndex = -1;
            species = 0;
            for (i = 0; i < count; i++) {
                if (fn_801429E8(field) != 0) {
                    visibleIndex++;
                    if (visibleIndex >= cursorIndex) {
                        species = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }
            if ((u16)species != 0) {
                lbl_8047A2E8 = cursorIndex;
                soundId = 0x24;
            }
        }
    } else {
        if ((SUMMARY_ITEM_U16(input, 0x04) & 0xD0) != 0) {
            field = list;
            visibleIndex = -1;
            species = 0;
            for (i = 0; i < count; i++) {
                if (fn_801429E8(field) != 0) {
                    visibleIndex++;
                    if (visibleIndex >= cursorIndex) {
                        species = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }
            if ((u16)species != 0) {
                if (SUMMARY_ENTRY_FIELD(entry) == -1) {
                    fn_80129514((void*)lbl_8047A2F8, (u16)lbl_8047A2E8, (u16)cursorIndex);
                } else {
                    fn_80129948((void*)lbl_8047A2F8, (u8)SUMMARY_ENTRY_FIELD(entry),
                                 (u16)lbl_8047A2E8, (u16)cursorIndex);
                }
                soundId = 0x24;
            } else {
                soundId = 0x25;
            }
            lbl_8047A2E8 = -1;
        } else if ((SUMMARY_ITEM_U16(input, 0x04) & 0x20) != 0) {
            lbl_8047A2E8 = -1;
            soundId = 0x25;
        }
    }

    pageIndex = SUMMARY_CTX_S8(ctx, 0x95);
    packed = (u16)(cursorBiosGetPos(
                       (u16)*(u32*)(labelBase +
                                   pageIndex * SUMMARY_ENTRY_STRIDE)) >>
                   16);
    dataSource = *(s32*)(dataSourceBase +
                         pageIndex * SUMMARY_ENTRY_STRIDE);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8,
                                               (u8)dataSource, &count,
                                               0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8,
                                           &count, 0, 0, 0);
    }
    field = list;
    validCount = 0;
    for (i = 0; i < count; i++) {
        if (fn_801429E8(field) != 0) {
            validCount++;
        }
        field = (u8*)field + 4;
    }

    currentPacked = packed;
    baseIndex = (s32)(s8)((u8*)&currentPacked)[0];

    if (((SUMMARY_ITEM_U16(input, 0x04) | SUMMARY_ITEM_U16(input, 0x08)) & 2) != 0) {
        if ((s32)(s8)((u8*)&currentPacked)[1] + 1 + baseIndex < validCount + 1) {
            ((u8*)&currentPacked)[1]++;
            if ((s32)(s8)((u8*)&currentPacked)[1] >= 8) {
                ((u8*)&currentPacked)[0]++;
                ((u8*)&currentPacked)[1]--;
                lbl_8047A2C8 = 1;
                SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B788));
            } else {
                lbl_8047A2C8 = 0;
                SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B788));
            }
            soundId = 0x23;
            moved = 1;
        }
    }

    if (((SUMMARY_ITEM_U16(input, 0x04) | SUMMARY_ITEM_U16(input, 0x08)) & 1) != 0) {
        if ((s32)(s8)((u8*)&currentPacked)[1] > 0 || (s32)(s8)((u8*)&currentPacked)[0] > 0) {
            ((u8*)&currentPacked)[1]--;
            if ((s32)(s8)((u8*)&currentPacked)[1] < 0) {
                ((u8*)&currentPacked)[1] = 0;
                ((u8*)&currentPacked)[0]--;
                lbl_8047A2C8 = 1;
                SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B78C));
            } else {
                lbl_8047A2C8 = 0;
                SUMMARY_STORE_F32(lbl_8047A2D0, SUMMARY_F32(lbl_8047B78C));
            }
            soundId = 0x23;
            moved = 1;
        }
    }

    cursorBiosSetPos(
        (u16)*(u32*)(labelBase + pageIndex * SUMMARY_ENTRY_STRIDE),
        &currentPacked);

    if ((s32)lbl_8047A2E8 < 0 && moved == 0) {
        if ((SUMMARY_ITEM_U16(input, 0x06) & 8) != 0) {
            ctx[0x95]--;
            if (SUMMARY_CTX_S8(ctx, 0x95) < 1) {
                ctx[0x95] = 5;
            }
            SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047B750));
        }
        if ((SUMMARY_ITEM_U16(input, 0x06) & 4) != 0) {
            ctx[0x95]++;
            if (SUMMARY_CTX_S8(ctx, 0x95) >= 6) {
                ctx[0x95] = 1;
            }
            SUMMARY_STORE_F32(lbl_8047A2D4, SUMMARY_F32(lbl_8047B790));
        }
    }

    if (soundId != 0) {
        fn_80166A50(soundId, 0, 0xFF, 0);
    }

    if ((s32)lbl_8047A2E8 >= 0) {
        lbl_8047A2DC = 0x2B2B;
    } else {
        pageIndex = SUMMARY_CTX_S8(ctx, 0x95);
        entry = SUMMARY_ENTRY_RAW(pageIndex);
        packed = (u16)(cursorBiosGetPos(
                           (u16)*(u32*)(labelBase +
                                       pageIndex * SUMMARY_ENTRY_STRIDE)) >>
                       16);
        cursorIndex = (s32)(s8)((u8*)&packed)[0] + (s32)(s8)((u8*)&packed)[1];
        dataSource = SUMMARY_ENTRY_FIELD(entry);
        if (dataSource >= 0) {
            list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
        } else {
            list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
        }
        field = list;
        visibleIndex = -1;
        species = 0;
        for (i = 0; i < count; i++) {
            if (fn_801429E8(field) != 0) {
                visibleIndex++;
                if (visibleIndex >= cursorIndex) {
                    species = itemBiosGetItemDataId(field);
                    break;
                }
            }
            field = (u8*)field + 4;
        }
        if ((u16)species != 0) {
            itemDataBiosGetPtr((u16)species);
            lbl_8047A2DC = itemDataBiosGetDoc();
        } else {
            lbl_8047A2DC = fallbackLabels[lbl_8047A2E0];
        }
    }

    return 0;
}
#pragma pop
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
#pragma push
#pragma peephole off
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
#pragma pop
#endif

/* fn_80017868 - 0x80017868 | size: 0x84 */
extern u32 heroGetStatus(u32 a, s32 b, s32 c);
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2E0;
#if 0
asm void fn_80017868(void) {
#include "src/game/gs_pokemon_summary_fn_80017868.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80017868(u32 unused, u8* ctx) {
    u32 r;
    r = heroGetStatus(lbl_8047A2F8, 0xC, 0);
    if ((s32)lbl_8047A2E0 != 3) return 0;
    msgctrlSetValue(0x50, (s32)r);
    fn_800FB680((s32)*(s16*)(ctx + 0x54) - (s32)(s16)(GSmsgGetRect(0x151) >> 16), 0, -1, 0x151);
    return 0;
}
#pragma pop
#endif

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

/* fn_8001793C - 0x8001793C | size: 0x54 */
#if 0
asm void fn_8001793C(void) {
#include "src/game/gs_pokemon_summary_fn_8001793C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8001793C(SummaryPageContext* ctx, SummaryDrawItem* item) {
    item->color[0] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 0);
    item->color[1] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 1);
    item->color[2] = SUMMARY_PAGE_DISPLAY_COLOR(SUMMARY_PAGE_INDEX_VOLATILE(ctx), 2);
    return 0;
}
#pragma pop
#endif

/* fn_80017990 - 0x80017990 | size: 0x7c */
#if 0
asm void fn_80017990(void) {
#include "src/game/gs_pokemon_summary_fn_80017990.inc"
}
#else
#pragma push
#pragma peephole off
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
#pragma pop
#endif

/* fn_80017A0C - 0x80017A0C | size: 0x2ac */
extern u32 lbl_8047A2D8;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2D4;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
extern u32 lbl_8047A2C4;
extern u32 lbl_8047A2CC;
extern u32 lbl_8047B794;
extern u32 lbl_8047B798;
extern u32 lbl_8047B74C;
extern u32 lbl_8047B744;
extern u32 lbl_8047B79C;
#if 0
asm void fn_80017A0C(void) {
#include "src/game/gs_pokemon_summary_fn_80017A0C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80017A0C(u8* ctx) {
    u8* entry;
    void* list;
    s32 packed;
    u16 count;
    s32 dataSource;
    u32 species;
    s32 visibleIndex;
    s32 i;
    void* field;
    s32 cursorIndex;

    switch (SUMMARY_CTX_S8(ctx, 0x01)) {
    case 0:
        if (SUMMARY_CTX_S8(ctx, 0x02) == 0) {
            winSeqSetMenu(0x59, 0x5E);

            entry = SUMMARY_ENTRY_RAW(lbl_8047A2D8);
            *(u16*)&packed = (u16)(cursorBiosGetPos((u16)SUMMARY_ENTRY_LABEL(entry)) >> 16);
            entry = SUMMARY_ENTRY_RAW(SUMMARY_CTX_S8(ctx, 0x95));
            cursorIndex = (s32)(s8)((u8*)&packed)[0] + (s32)(s8)((u8*)&packed)[1];
            dataSource = SUMMARY_ENTRY_FIELD(entry);
            if (dataSource >= 0) {
                list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
            } else {
                list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
            }

            field = list;
            visibleIndex = -1;
            species = 0;
            for (i = 0; i < count; i++) {
                if (fn_801429E8(field) != 0) {
                    visibleIndex++;
                    if (visibleIndex >= cursorIndex) {
                        species = itemBiosGetItemDataId(field);
                        break;
                    }
                }
                field = (u8*)field + 4;
            }
            itemDataBiosGetPtr((u16)species);
            lbl_8047A2DC = itemDataBiosGetDoc();
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
        SUMMARY_STORE_F32(lbl_8047A2CC, SUMMARY_F32(lbl_8047A2CC) + SUMMARY_F32(lbl_8047B79C));
        if (SUMMARY_F32(lbl_8047A2CC) >= SUMMARY_F32(lbl_8047B744)) {
            SUMMARY_STORE_F32(lbl_8047A2CC, SUMMARY_F32(lbl_8047A2CC) - SUMMARY_F32(lbl_8047B744));
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

/* fn_80017CB8 - 0x80017CB8 | size: 0x1d4 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2E0;
#if 0
asm void fn_80017CB8(void) {
#include "src/game/gs_pokemon_summary_fn_80017CB8.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80017CB8(u8* outEntries, s32 maxEntries, s32 pageIndex, s32 selectedIndex) {
    u8* entry;
    u8* sourceEntries;
    void* list;
    void* field;
    u16 count;
    s32 dataSource;
    s32 i;
    s32 visibleIndex;
    s32 outCount;
    s32 sourceCount;
    s32 hasRibbonState;
    s32 speciesIsNotEgg;
    u16 flags;
    u32 species;

    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource >= 0) {
        list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
    }

    field = list;
    visibleIndex = -1;
    species = 0;
    for (i = 0; i < count; i++) {
        if (fn_801429E8(field) != 0) {
            visibleIndex++;
            if (visibleIndex >= selectedIndex) {
                species = itemBiosGetItemDataId(field);
                break;
            }
        }
        field = (u8*)field + 4;
    }

    itemDataBiosGetPtr((u16)species);
    if (itemDataBiosGetBattleUseFunc() != 0) {
        hasRibbonState = 1;
    } else {
        hasRibbonState = 0;
    }
    speciesIsNotEgg = ((u16)species != 0x219);

    sourceEntries = *(u8**)(entry + 0x24 + (s32)lbl_8047A2E0 * 8);
    sourceCount = *(s32*)(entry + 0x28 + (s32)lbl_8047A2E0 * 8);
    outCount = 0;
    for (i = 0; i < sourceCount && outCount < maxEntries; i++) {
        flags = *(u16*)(sourceEntries + 8);
        if ((flags & 1) == 0 &&
            ((flags & 2) == 0 || hasRibbonState != 0) &&
            ((flags & 8) == 0 || speciesIsNotEgg != 0)) {
            *(u32*)(outEntries + 0) = *(u32*)(sourceEntries + 0);
            *(u32*)(outEntries + 4) = *(u32*)(sourceEntries + 4);
            *(u16*)(outEntries + 8) = 0;
            if (hasRibbonState == 0 && (flags & 4) != 0) {
                *(u16*)(outEntries + 8) |= 1;
            }
            outCount++;
            outEntries += 0x0C;
        }
        sourceEntries += 0x0C;
    }

    return outCount;
}
#pragma pop
#endif

/* fn_80017E8C - 0x80017E8C | size: 0x338 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2FC;
#if 0
asm void fn_80017E8C(void) {
#include "src/game/gs_pokemon_summary_fn_80017E8C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80017E8C(s32 pageIndex, u16 species, s16 slotIndex) {
    u8* entry;
    void* list;
    void* field;
    u16 count;
    s32 dataSource;
    s32 mode;
    s32 menuId;
    s32 menuResult;
    s32 result;
    s32 i;
    s32 visibleIndex;
    u32 selectedValue;
    u32 menuArg[5];

    /* (u16) is a no-op on a u16 param, but it is what makes MWCC emit
       retail's clrlwi here; the same cast at the inner call site does not. */
    itemDataBiosGetPtr((u16)species);
    /* retail masks to u8 then compares *signed*; an inline (u8)x == 0 gives
       cmplwi instead. */
    result = (u8)fn_80143F9C();
    if (result == 0) {
        msgctrlSetValue(0x2D, species);
        winMsgOpen(2, 0x426C, 1, 0);
        winMsgClose(1);
        return 0;
    }

    entry = SUMMARY_ENTRY_RAW(pageIndex);
    dataSource = SUMMARY_ENTRY_FIELD(entry);
    if (dataSource != 5 && dataSource != -1) {
        lbl_8047A2DC = 0x2B28;
        heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, 0, &count, 0, 0);
        if (count > 100) {
            mode = 3;
        } else {
            mode = 2;
        }

        if (dataSource >= 0) {
            list = heroItemGetItemKindToItemAryPtr((void*)lbl_8047A2F8, (u8)dataSource, &count, 0, 0, 0);
        } else {
            list = heroHizukiItemGetItemAryPtr((void*)lbl_8047A2F8, &count, 0, 0, 0);
        }

        field = list;
        visibleIndex = -1;
        selectedValue = 0;
        for (i = 0; i < count; i++) {
            if (fn_801429E8(field) != 0) {
                visibleIndex++;
                if (visibleIndex >= slotIndex) {
                    selectedValue = itemBiosGetNum(field);
                    break;
                }
            }
            field = (u8*)field + 4;
        }

        if ((u16)selectedValue < 1) {
            result = 0;
        } else {
            if (mode == 2) {
                menuId = 0x5B;
                menuArg[0] = 1;
            } else {
                menuId = 0x5C;
                menuArg[0] = 2;
            }
            ((u8*)menuArg)[4] = entry[0];
            ((u8*)menuArg)[5] = entry[1];
            ((u8*)menuArg)[6] = entry[2];
            menuArg[1] = menuId;
            menuArg[2] = selectedValue;
            menuArg[3] = 1;
            menuArg[4] = 0;
            lbl_8047A2FC = 1;
            menuResult = menuOpenCustom(menuId, windowGetActiveID(), &mode, 0, 1, 1, menuArg);
            menuClose(menuId);
            menuCloseSync(menuId, 1);
            if (menuResult == -1) {
                result = -1;
            } else {
                result = lbl_8047A2FC;
            }
        }
    } else {
        result = 1;
    }

    if (result < 0) {
        return 0;
    }

    /* Retail emits pcboxDelItem immediately after the capacity test and the
     * failure body after it, i.e. the success arm is written first. */
    if ((u16)pcboxGetItemCapacity(0, species) >= result) {
        pcboxDelItem(0, species, (u16)result);
    } else {
        msgctrlSetValue(0x2D, species);
        winMsgOpen(2, 0x2B49, 1, 0);
        winMsgClose(1);
        return 0;
    }

    if (dataSource == -1) {
        fn_8012959C((void*)lbl_8047A2F8, species, (u16)result, slotIndex);
    } else {
        heroItemDecItemDataId((void*)lbl_8047A2F8, species, (u16)result, slotIndex);
    }

    msgctrlSetValue(0x2D, species);
    msgctrlSetValue(0x2F, result);
    winMsgOpen(2, 0x4266, 1, 0);
    winMsgClose(1);
    return 1;
}
#pragma pop
#endif
