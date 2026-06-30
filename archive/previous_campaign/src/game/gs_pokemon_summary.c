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
 * data fields and renders them using fn_80129BC8 (get Pokemon field data)
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
extern void* fn_80129BC8(void* pokeData, u8 fieldId, u16* outCount,
                          s32 p4, s32 p5, s32 p6);
extern void* fn_801297D8(void* pokeData, u16* outCount, s32 p3, s32 p4, s32 p5);
extern u8    fn_801429E8(void* fieldData);    /* Check field validity */
extern u16   fn_80143C50(void* fieldData);    /* Get field value */
extern u16   fn_801440A0(u16 speciesId);      /* Get species data */
extern u16   fn_80143FFC(void);               /* Get display field count */

/* Text rendering */
extern void  fn_80132A38(s32 paramId, s32 value);
extern u32   fn_800FA444(s32 resourceId);
extern void  fn_800FB680(s32 x, s32 y, s32 flags, u32 color);

/* Math/rendering helpers */
extern f32   sin(f32);
extern f32   cos(f32);
extern void  fn_800E0CA0(f32 angle);          /* Set camera rotation */
extern void  fn_800E090C(void* outVec, void* posA, void* posB); /* Vector subtract */
extern void  fn_80106ADC(s32 p1, void* data, s32 p3, s32 p4, u8 p5);

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
extern u32 fn_80103E68(u32 a);
extern u8 lbl_80266918[];
#define sSummaryPageEntries lbl_80266918
typedef struct SummaryPageEntry {
    u8 displayColor[3];      /* 0x00, copied to output bytes 0x64-0x66 */
    u8 unk_03;
    s32 dataSource;          /* 0x04, -1 uses default party source */
    u8 unk_08[8];
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
#if 0
asm void fn_80015050(void) {
#include "src/game/gs_pokemon_summary_fn_80015050.inc"
}
#else
#pragma push
#pragma peephole off
typedef s32 (*DrawHandlerFn)(u8*, u8*, s16*);
s32 fn_80015050(u8* src, u8* param) {
    DrawHandlerFn fp;
    u16 tmp;
    u8* entry = (u8*)sSummaryPageEntries;
    entry += (s32)(s8)src[0x95] * 0x4C;
    fp = *(DrawHandlerFn*)(entry + 0x18);
    if (fp != NULL) {
        tmp = fn_80103E68((u16)*(u32*)(entry + 0x1C)) >> 16;
        return fp(src, param, (s16*)&tmp);
    }
    return 0;
}
#pragma pop
#endif

/* fn_800150E4 - 0x800150E4 | size: 0x290 */
extern void fn_80143C68(void);
extern void fn_80144088(void);
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 1
asm void fn_800150E4(void) {
#include "src/game/gs_pokemon_summary_fn_800150E4.inc"
}
#else
void fn_800150E4(void) { /* TODO */ }
#endif

/* fn_80015374 - 0x80015374 | size: 0x23c */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 1
asm void fn_80015374(void) {
#include "src/game/gs_pokemon_summary_fn_80015374.inc"
}
#else
void fn_80015374(void) { /* TODO */ }
#endif

/* fn_800155B0 - 0x800155B0 | size: 0x40c */
extern void fn_80143E60(void);
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 1
asm void fn_800155B0(void) {
#include "src/game/gs_pokemon_summary_fn_800155B0.inc"
}
#else
void fn_800155B0(void) { /* TODO */ }
#endif

/* fn_800159BC - 0x800159BC | size: 0x480 */
extern void fn_80143F24(void);
extern void fn_80143E88(void);
extern void fn_80143EF0(void);
extern void fn_8011CA34(void);
extern void fn_8011CA1C(void);
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 1
asm void fn_800159BC(void) {
#include "src/game/gs_pokemon_summary_fn_800159BC.inc"
}
#else
void fn_800159BC(void) { /* TODO */ }
#endif

/* fn_80015E3C - 0x80015E3C | size: 0x374 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047B748;
extern u32 lbl_8047A2D0;
extern u32 lbl_8047A2C8;
#if 1
asm void fn_80015E3C(void) {
#include "src/game/gs_pokemon_summary_fn_80015E3C.inc"
}
#else
void fn_80015E3C(void) { /* TODO */ }
#endif

/* fn_800161B0 - 0x800161B0 | size: 0x198 */
extern void fn_800FE38C(void);
extern void fn_800FE35C(void);
extern u8 lbl_802EF0A8[];
extern u32 lbl_8047A2D4;
extern u32 lbl_8047B748;
extern u32 lbl_8047B750;
#if 1
asm void fn_800161B0(void) {
#include "src/game/gs_pokemon_summary_fn_800161B0.inc"
}
#else
void fn_800161B0(void) { /* TODO */ }
#endif

/* fn_80016348 - 0x80016348 | size: 0x188 */
extern u32 lbl_8047A2D4;
extern u32 lbl_8047B748;
extern u32 lbl_8047B750;
#if 1
asm void fn_80016348(void) {
#include "src/game/gs_pokemon_summary_fn_80016348.inc"
}
#else
void fn_80016348(void) { /* TODO */ }
#endif

/* fn_800164D0 - 0x800164D0 | size: 0x148 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2D8;
extern u32 lbl_8047B744;
extern u32 lbl_8047A2C4;
extern u32 lbl_8047B740;
#if 1
asm void fn_800164D0(void) {
#include "src/game/gs_pokemon_summary_fn_800164D0.inc"
}
#else
void fn_800164D0(void) { /* TODO */ }
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
    u16 tmp;
    tmp = (u16)(fn_80103E68((u16)*(u32*)(&sSummaryPageEntries[(s32)(s8)src[0x95] * 0x4C + 0x1C])) >> 16);
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
#if 1
asm void fn_800166BC(void) {
#include "src/game/gs_pokemon_summary_fn_800166BC.inc"
}
#else
void fn_800166BC(void) { /* TODO */ }
#endif

/* fn_800167D0 - 0x800167D0 | size: 0x2ec */
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern void fn_80104160(void);
extern u32 lbl_8047B748;
extern u32 lbl_8047B744;
extern u32 lbl_8047B768;
extern u32 lbl_8047B75C;
extern u32 lbl_8047B770;
extern u32 lbl_8047B774;
extern u32 lbl_8047B778;
#if 1
asm void fn_800167D0(void) {
#include "src/game/gs_pokemon_summary_fn_800167D0.inc"
}
#else
void fn_800167D0(void) { /* TODO */ }
#endif

/* fn_80016ABC - 0x80016ABC | size: 0x458 */
extern void fn_800FE6D0(void);
extern void fn_800FE4D4(void);
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
#if 1
asm void fn_80016ABC(void) {
#include "src/game/gs_pokemon_summary_fn_80016ABC.inc"
}
#else
void fn_80016ABC(void) { /* TODO */ }
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
    tmp = (u16)(fn_80103E68((u16)*(u32*)(&sSummaryPageEntries[(s32)(s8)src[0x95] * 0x4C + 0x1C])) >> 16);
    if ((s32)lbl_8047A2E8 >= 0) {
        v = ((s32)lbl_8047A2E8 - (s32)(s8)*(u8*)&tmp) * 0x1f + 0x95;
        if ((s32)lbl_8047A2C8 != 0) {
            v -= (s32)*(f32*)&lbl_8047A2D0;
        }
        if (v + (s32)*(s16*)(dst + 0x56) < 0x95) {
            col = 0;
        } else if (v < 0x18d) {
            col = 0xff;
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
extern void fn_80105624(void);
extern void fn_80129514(void);
extern void fn_80129948(void);
extern void fn_80103EAC(void);
extern void fn_80166A50(void);
extern void fn_80143F84(void);
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
#if 1
asm void fn_80017028(void) {
#include "src/game/gs_pokemon_summary_fn_80017028.inc"
}
#else
void fn_80017028(void) { /* TODO */ }
#endif

/* fn_80017764 - 0x80017764 | size: 0x2c */
extern void fn_80102ED4(void);
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2E8;
extern void fn_80102ED4(void);

void fn_80017764(void) {
    if ((s32)lbl_8047A2E8 < 0) {
        fn_80102ED4();
    }
}

/* fn_80017790 - 0x80017790 | size: 0xd8 | WALL 85%: regalloc (ptr-chasing vs indexed) + scheduling */
#if 1
asm void fn_80017790(void) {
#include "src/game/gs_pokemon_summary_fn_80017790.inc"
}
#else
void fn_80017790(void) { /* TODO */ }
#endif

/* fn_80017868 - 0x80017868 | size: 0x84 */
extern u32 fn_8012A5B0(u32 a, s32 b, s32 c);
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
    r = fn_8012A5B0(lbl_8047A2F8, 0xC, 0);
    if ((s32)lbl_8047A2E0 != 3) return 0;
    fn_80132A38(0x50, (s32)r);
    fn_800FB680((s32)*(s16*)(ctx + 0x54) - (s32)(s16)(fn_800FA444(0x151) >> 16), 0, -1, 0x151);
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
extern void fn_801080CC(void);
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
#if 1
asm void fn_80017A0C(void) {
#include "src/game/gs_pokemon_summary_fn_80017A0C.inc"
}
#else
void fn_80017A0C(void) { /* TODO */ }
#endif

/* fn_80017CB8 - 0x80017CB8 | size: 0x1d4 */
extern void fn_80143DCC(void);
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2E0;
#if 1
asm void fn_80017CB8(void) {
#include "src/game/gs_pokemon_summary_fn_80017CB8.inc"
}
#else
void fn_80017CB8(void) { /* TODO */ }
#endif

/* fn_80017E8C - 0x80017E8C | size: 0x338 */
extern void fn_80143F9C(void);
extern void fn_80106D3C(void);
extern void fn_801069FC(void);
extern void fn_801046B8(void);
extern void fn_801026A4(void);
extern void fn_80102510(void);
extern void menuCloseSync(void);
extern void fn_80134420(void);
extern void fn_8013467C(void);
extern void fn_8012959C(void);
extern void fn_801299C8(void);
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2FC;
#if 1
asm void fn_80017E8C(void) {
#include "src/game/gs_pokemon_summary_fn_80017E8C.inc"
}
#else
void fn_80017E8C(void) { /* TODO */ }
#endif

