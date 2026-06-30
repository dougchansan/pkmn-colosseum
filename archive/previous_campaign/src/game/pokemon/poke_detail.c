/**
 * @file poke_detail.c
 * @brief Pokemon detail sub-screens and data rendering utilities.
 *
 * Contains the rendering and data-display functions for individual Pokemon
 * detail views -- move info panels, stat breakdowns, nature/characteristic
 * display, held item info, and similar sub-screen elements that are embedded
 * within the summary and party screens.
 *
 * Key behaviors:
 *   - 46 functions with NO direct BSS references, indicating these operate
 *     purely on caller-provided pointers (typically the summary state struct
 *     from menu_pokemon.c)
 *   - Many functions call fn_8004BE40 (a formatting/rendering helper within
 *     this module) repeatedly, suggesting a template-based rendering pattern
 *   - fn_8004C120 dispatches to fn_8004D34C and fn_8004D9C0 for different
 *     detail page types
 *   - fn_8004D34C calls fn_8004BFB0, fn_8004DC18, fn_8004DFCC for specific
 *     data rendering
 *   - Functions fn_8004C6C0 through fn_8004D8BC form a series of similar-sized
 *     blocks that each call fn_8004BE40, suggesting per-stat or per-field
 *     rendering routines (ATK, DEF, SPD, etc.)
 *   - fn_8004D9C0 calls fn_8004BE40 and fn_8004E9C0 for complex data display
 *   - The later functions (fn_8004E144-fn_8004E9C0) appear to be standalone
 *     utility formatters (type name, nature name, ability display, etc.)
 *
 * Address range: 0x8004BE0C - 0x8004EADC (46 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  fn_800E01D0(void* dst, void* src); /* material copy */
extern void  fn_80109220(u32 obj, u8 visible);  /* model visibility */
extern void  fn_800D61E4(void* obj);            /* render matrix set */
extern void  fn_800D5CB8(void* obj);            /* render cleanup */

/* ===== Pokemon data ===== */
extern void* fn_801FAA58(u32 slot);
extern u32   fn_801FB1C0(void* pkmn, u32 field);
extern void* fn_801FBFBC(u16 species);

/* ===== Text / Messages ===== */
extern void* fn_8001E224(u32 msgBank, u32 msgId);
extern u32   fn_8001E200(u32 msgBank, u32 msgId);

/*
 * Functions in this translation unit (46 total):
 *
 * fn_8004BE0C  0x034  Detail utility A
 * fn_8004BE40  0x050  Detail render helper (called by many in this TU)
 * fn_8004BE90  0x090  Detail formatter A
 * fn_8004BF20  0x090  Detail formatter B
 * fn_8004BFB0  0x170  Detail data renderer (called by fn_8004D34C)
 * fn_8004C120  0x1B8  Detail page dispatcher (calls fn_8004D34C, fn_8004D9C0)
 * fn_8004C2D8  0x094  Detail sub-renderer A
 * fn_8004C36C  0x078  Detail sub-renderer B
 * fn_8004C3E4  0x0C0  Detail sub-renderer C
 * fn_8004C4A4  0x10C  Detail sub-renderer D
 * fn_8004C5B0  0x110  Detail sub-renderer E
 * fn_8004C6C0  0x1EC  Per-stat renderer (calls fn_8004BE40) -- ATK
 * fn_8004C8AC  0x1EC  Per-stat renderer (calls fn_8004BE40) -- DEF
 * fn_8004CA98  0x1A0  Per-stat renderer (calls fn_8004BE40) -- SP.ATK
 * fn_8004CC38  0x1A0  Per-stat renderer (calls fn_8004BE40) -- SP.DEF
 * fn_8004CDD8  0x1A0  Per-stat renderer (calls fn_8004BE40) -- SPEED
 * fn_8004CF78  0x2F4  Per-stat renderer (extended)
 * fn_8004D26C  0x0E0  Stat sub-helper
 * fn_8004D34C  0x244  Detail data renderer main (calls fn_8004BFB0, fn_8004DC18, fn_8004DFCC)
 * fn_8004D590  0x05C  Stat accessor (calls fn_8004BE40)
 * fn_8004D5EC  0x060  Stat accessor
 * fn_8004D64C  0x060  Stat accessor
 * fn_8004D6AC  0x044  Stat accessor
 * fn_8004D6F0  0x070  Stat accessor
 * fn_8004D760  0x070  Stat accessor
 * fn_8004D7D0  0x0EC  Stat accessor (extended)
 * fn_8004D8BC  0x06C  Stat accessor
 * fn_8004D928  0x098  Stat display helper
 * fn_8004D9C0  0x0A4  Detail page renderer B (calls fn_8004BE40, fn_8004E9C0)
 * fn_8004DA64  0x0D0  Nature/characteristic formatter
 * fn_8004DB34  0x04C  Small accessor
 * fn_8004DB80  0x098  Ability name formatter
 * fn_8004DC18  0x0A8  Move detail renderer (called by fn_8004D34C)
 * fn_8004DCC0  0x100  Move type/power display
 * fn_8004DDC0  0x174  Move PP display
 * fn_8004DF34  0x098  Move category display
 * fn_8004DFCC  0x178  Contest move detail (called by fn_8004D34C)
 * fn_8004E144  0x03C  Small formatter
 * fn_8004E180  0x160  Type effectiveness formatter
 * fn_8004E2E0  0x160  Type name formatter
 * fn_8004E440  0x0D0  Nature name formatter
 * fn_8004E510  0x280  Held item info display
 * fn_8004E790  0x10C  Ribbon info display
 * fn_8004E89C  0x044  Small accessor
 * fn_8004E8E0  0x0E0  Marking display
 * fn_8004E9C0  0x11C  Complex data formatter (called by fn_8004D9C0)
 */


/* 0x8004BE0C | size: 0x34 */
extern void fn_800FF730(u32);
extern void fn_8011288C(u32, u32);

#pragma push
#pragma peephole off
void fn_8004BE0C(void) {
    fn_800FF730(0x392);
    fn_8011288C(0, 0);
    _threadSwitch();
}
#pragma pop

/* 0x8004C120 | size: 0x1B8 */
/*
 * WALL @ 99.32% (band scratch, jun24). classify_residual verdict: REG-COLORING
 * (winnable per classifier, but decl-order space is provably exhausted — see
 * below). Build uses the asm-include so it stays byte-exact. Faithful real C
 * that reaches 99.32% is kept below for reference and lives in
 * tools/decomp_work/scratch/band_pl_poke_detail.c.
 *
 * Progress this session: prior note claimed SHAPE/90.86%/not-winnable — that was
 * stale. With NAMED locals (no rNN), the residual is pure register-coloring:
 *   - Function-scope decl order [page, handle, count, ptr] fixes page=r30,
 *     handle=r29 and lands the block temps; raised 90.86 -> 97.95%.
 *   - case2/case3 block-temp decl order [w, i, n] (i in the middle -> r26 in
 *     both; 1st/3rd swap positionally so case2 w=r27/n=r25, case3 w=r25/n=r27)
 *     fixed the "case2/3 temp permutation"; raised 97.95 -> 99.32%.
 *
 * Remaining residual (the ONLY diff): a count<->ptr non-volatile swap. OURS gets
 * count=r31, ptr=r28; target wants ptr=r31 (heavily used in the switch),
 * count=r28 (lightly used). Across 3 measured declaration orders, `count` (the
 * first call-defined survivor) is PINNED to r31 and the other three survivors
 * fill r30/r29/r28 in decl order — so NO declaration order can put ptr in r31
 * while count exists. The definition-form / chained `ptr = lbl = fn()` lever
 * produced identical codegen (no effect). This is a register-PERMUTATION wall
 * (same family as colquery E64), not currently steerable by the source levers.
 *
 * void fn_8004C120(void) {
 *     s32 page; u16 handle; s32 count; u16* ptr;       // decl order matters
 *     page = 0;
 *     count = fn_801D1F7C();
 *     if (count > 0) {
 *         handle = fn_800E3534(count * 2);
 *         ptr = (u16*)(lbl_8047A500 = fn_800E27B0(handle));
 *         switch (fn_801D1B4C()) {
 *         case 1: { s32 i; for (i = 0; i < fn_801D1F7C(); i++) { *ptr = (u16)fn_801D1F0C(i); ptr++; } break; }
 *         case 2: { u16* w; s32 i; s32 n; n = fn_801D1F7C(); w = ptr;
 *                   for (i = 0; i < n; i++) { *w = (u16)fn_801D1F0C(i); w++; }
 *                   fn_800CA620(ptr, n, 2, fn_8004BF20); break; }
 *         case 3: { u16* w; s32 i; s32 n; n = fn_801D1F7C(); w = ptr;
 *                   for (i = 0; i < n; i++) { *w = (u16)fn_801D1F0C(i); w++; }
 *                   fn_800CA620(ptr, n, 2, fn_8004BE90); break; }
 *         case 0: default: { s32 j; for (j = fn_801D1F7C() - 1; j >= 0; j--) { *ptr = (u16)fn_801D1F0C(j); ptr++; } break; }
 *         }
 *     } else {
 *         lbl_8047A500 = NULL;
 *     }
 *     for (;;) { if (fn_8004D34C(page) < 0) break; page = fn_8004D9C0(); }
 *     if (count > 0) { fn_800E24B0(handle); fn_800E209C(handle); }
 * }
 */
asm void fn_8004C120(void) { nofralloc
    #include "asm/GC6E01/nonmatching/poke_detail/fn_8004C120.s"
}

