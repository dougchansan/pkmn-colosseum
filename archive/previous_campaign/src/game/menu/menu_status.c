/**
 * @file menu_status.c
 * @brief Status screen and stat display subsystem.
 *
 * Handles the various status display screens including the player's
 * trainer card, Pokemon stat displays with bar graphs, and data
 * formatting for the game's numerical displays. This module provides
 * the rendering pipeline for displaying stats, IVs/EVs, and other
 * numerical Pokemon data in formatted screen layouts.
 *
 * Key behaviors:
 *   - No BSS references, indicating these functions operate on
 *     caller-provided state structures
 *   - fn_800552D4 (0x5E4 bytes) and fn_800558B8 are related state machines
 *     that both call fn_80054680, fn_800546C0, fn_800546F0 (script accessor
 *     stubs from script_callback.c)
 *   - fn_80055B98 aggregates results from fn_8005471C, fn_80054760, and
 *     fn_800558B8
 *   - fn_80056084 (0x58C bytes) manages status bar rendering with calls
 *     to fn_80057DE8 and fn_80057F94 (dialog system)
 *   - fn_80054C44 dispatches to fn_800566E8, fn_80056704, fn_800567AC
 *     (text rendering helpers in msgbox.c)
 *   - fn_80054E7C and fn_80054EC8 handle different stat page types
 *   - fn_800550B4 coordinates with fn_80054EC8 and fn_80056854
 *   - fn_80055EB8 handles status effect indicators
 *   - Functions fn_80054914 through fn_80054B1C are a set of 7 small
 *     accessor functions (0x0DC, 0x03C, 0x03C, 0x03C, 0x03C, 0x03C, 0x128)
 *     likely mapping stat indices to display parameters
 *
 * Address range: 0x80054914 - 0x80056C54 (35 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_800D61E4(void* obj);
extern void  fn_800D5CB8(void* obj);
extern void  fn_80109220(u32 obj, u8 visible);
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d);

/* ===== Text / Dialog (internal) ===== */
extern void  fn_80057DE8(void* ctx);
extern void  fn_80057F94(void* ctx);
extern void  fn_80057094(void);
extern void  fn_80057A08(void);

/* ===== Script accessors (internal) ===== */
extern void  fn_80054680(void);
extern void  fn_800546C0(void);
extern void  fn_800546F0(void);
extern void  fn_8005471C(u32 a);
extern void  fn_80054760(void);
extern void  fn_80055194(void);

/* ===== Pokemon data ===== */
extern void* fn_801FAA58(u32 slot);
extern u32   fn_801FB1C0(void* pkmn, u32 field);

/* ===== Text / Messages ===== */
extern void* fn_8001E224(u32 msgBank, u32 msgId);
extern u32   fn_8001E200(u32 msgBank, u32 msgId);

/* ===== Rodata tables ===== */
extern const u32 lbl_80267398[];  /* Status display layout table (large) */
extern const u32 lbl_80267518[];  /* Stat bar value table (90 entries) */
extern const u32 lbl_80267680[];  /* Status page message IDs [0x65B, 0x683, 0x6AB] */
extern const u32 lbl_8026768C[];  /* Status icon indices [0x90, 0x91, 0x92] */
extern const u32 lbl_80267698[];  /* Status layout params (7 entries) */
extern const u32 lbl_802676B4[];  /* Status bar positions (15 entries) */
extern const u32 lbl_802676F0[];  /* Status bar color table (56 entries) */
extern const u32 lbl_802677D0[];  /* Status label table (28 entries) */

/* ===== Data section ===== */
extern const u8 lbl_802E51C8[];   /* Status screen model positions (8 * 6 floats) */
extern const f32 lbl_802E5288[];  /* Stat page position rects (4 * 2 floats) */
extern const f32 lbl_802E52A8[];  /* Rotation angle table [0, pi/2, pi, 3pi/2] */
extern const f32 lbl_802E52B8[];  /* Rotation angle table (alt order) */
extern const u8 lbl_802E52C8[];   /* Status screen layout data (0x12C bytes) */
extern const u8 lbl_802E53F4[];   /* Status effect sprite data A */
extern const u8 lbl_802E5400[];   /* Status effect sprite data B */
extern const u8 lbl_802E540C[];   /* Status effect sprite data C */
extern const u8 lbl_802E5418[];   /* Status effect sprite data D */
extern const u8 lbl_802E5424[];   /* Status effect sprite data E */
extern const u8 lbl_802E5430[];   /* Status effect sprite data F */
extern const u8 lbl_802E543C[];   /* Status effect sprite data G */
extern const u8 lbl_802E5448[];   /* Material preset data (shared w/ menu_pokemon) */
extern const u8 lbl_802E554C[];   /* Status screen extended layout */
extern const u8 lbl_802E60B0[];   /* Status screen format strings */
extern const u8 lbl_802E61D8[];   /* Status screen params A */
extern const u8 lbl_802E61E8[];   /* Status screen params B */
extern const u8 lbl_802EF0A8[];   /* Status screen data table */

/*
 * Functions in this translation unit (35 total):
 *
 * fn_80054914  0x0DC  Stat accessor/mapper A
 * fn_800549F0  0x03C  Stat accessor B
 * fn_80054A2C  0x03C  Stat accessor C
 * fn_80054A68  0x03C  Stat accessor D
 * fn_80054AA4  0x03C  Stat accessor E
 * fn_80054AE0  0x03C  Stat accessor F
 * fn_80054B1C  0x128  Stat accessor G (extended)
 * fn_80054C44  0x238  Status display dispatcher (calls fn_800566E8, fn_80056704, fn_800567AC)
 * fn_80054E7C  0x04C  Stat page type A handler
 * fn_80054EC8  0x1EC  Stat page type B handler (calls fn_80056A78, fn_80057400, fn_800574A8)
 * fn_800550B4  0x0E0  Stat page coordinator (calls fn_80054EC8, fn_80056854, fn_80057830)
 * fn_80055194  0x038  Small utility
 * fn_800551CC  0x108  Status formatter
 * fn_800552D4  0x5E4  Status state machine A (calls fn_80054680, fn_800546C0, fn_800546F0)
 * fn_800558B8  0x2E0  Status state machine B (calls fn_80054680, fn_800546C0, fn_800546F0)
 * fn_80055B98  0x094  Status aggregator (calls fn_8005471C, fn_80054760, fn_800558B8)
 * fn_80055C2C  0x0A8  Status helper A
 * fn_80055CD4  0x060  Status helper B
 * fn_80055D34  0x0AC  Status helper C
 * fn_80055DE0  0x030  Status helper D
 * fn_80055E10  0x028  Status helper E
 * fn_80055E38  0x080  Status helper F
 * fn_80055EB8  0x0D0  Status effect indicator handler
 * fn_80055F88  0x0FC  Status cleanup
 * fn_80056084  0x58C  Status bar renderer (calls fn_80057DE8, fn_80057F94)
 * fn_80056610  0x0A4  Text rendering helper A
 * fn_800566B4  0x024  Text rendering helper B
 * fn_800566D8  0x010  Text rendering stub
 * fn_800566E8  0x01C  Text rendering helper C
 * fn_80056704  0x0A8  Text rendering helper D
 * fn_800567AC  0x0A8  Text rendering helper E
 * fn_80056854  0x220  Text rendering block (calls fn_80056A78)
 * fn_80056A78  0x008  Text rendering tiny accessor
 * fn_80056A80  0x0F4  Text rendering complex
 * fn_80056B74  0x0E0  Text rendering finalize
 */


/* 0x80054914 | size: 0xDC */
asm void fn_80054914(void) { nofralloc
    #include "asm/GC6E01/nonmatching/menu_status/fn_80054914.s"
}

/* 0x800552D4 | size: 0x5E4 */
asm void fn_800552D4(void) { nofralloc
    #include "asm/GC6E01/nonmatching/menu_status/fn_800552D4.s"
}


/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 1 functions matched
 * =================================================================== */

extern u32 lbl_8047A584;

/* Address: 0x80056A78 | Size: 0x8 | Pattern: sda_getter */
u32 fn_80056A78(void) {
    return lbl_8047A584;
}
