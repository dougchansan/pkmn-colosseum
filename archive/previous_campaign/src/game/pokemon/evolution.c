/**
 * @file evolution.c
 * @brief Pokemon evolution sequence and animation system.
 *
 * Handles the evolution cutscene when a Pokemon levels up and evolves.
 * Manages the 3D model transition animation, sound effects, and the
 * post-evolution stat/move learning screens.
 *
 * Key behaviors:
 *   - Uses BSS lbl_803A6A60 (0x50 bytes) as the evolution state structure,
 *     containing animation progress (float at +0x28/+0x2C), current stage
 *     (byte at +0x00/+0x01), and timing flags (+0x49/+0x4A)
 *   - fn_800495C8 is the main evolution update function (0xC0 stack frame),
 *     which interpolates between the pre-evolution and post-evolution models
 *     using float lerp at offsets +0x08, +0x28, +0x2C in the state struct
 *   - Calls fn_800411EC to get the current party slot selection
 *   - fn_800499BC (0xAC0 = 2752 bytes) is the evolution state machine that
 *     coordinates the full sequence: model load, animation, learn-move check
 *   - fn_8004A7A8 (0xAD0 = 2768 bytes) handles post-evolution processing
 *     (stat recalc, move learning prompts)
 *   - fn_8004B7EC (0x5CC bytes) handles the evolution completion transition,
 *     calling back into the shop (fn_8003C7C0) and summary (fn_80044630)
 *     systems to update displayed data
 *   - fn_8004BDB8, fn_8004BDEC, fn_8004BDFC are small accessors for
 *     evolution result data
 *   - Model rendering via fn_801096F8, sound via fn_800FB680
 *
 * BSS usage:
 *   - lbl_803A6A60 (0x50 bytes): Evolution state structure
 *
 * Address range: 0x800495C8 - 0x8004BE0C (10 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  _threadSwitch(void);           /* GSthread yield */
extern void  fn_801096F8(u32 modelSlot);  /* Model show/hide */
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d); /* Sound trigger */
extern void  fn_80132A38(u32 effectId, u32 param);
extern void  fn_80109220(u32 obj, u8 visible);

/* ===== Pokemon data ===== */
extern void* fn_801FAA58(u32 slot);
extern u32   fn_801FB1C0(void* pkmn, u32 field);
extern void* fn_801FBFBC(u16 species);

/* ===== Internal cross-references ===== */
extern u32   fn_800411EC(void);           /* Get current party slot (in menu_pokemon.c) */
extern void  fn_8003C7C0(void* state);    /* Shop transaction (in menu_shop.c) */
extern void  fn_80044630(void* state);    /* Summary update (in menu_pokemon.c) */
extern void  fn_8004A47C(void);           /* Evolution helper (internal) */

/* ===== BSS data ===== */
extern u8    lbl_803A6A60[];   /* Evolution state (0x50 bytes) */

/* ===== SDA float constants ===== */
extern f32   lbl_8047BDA0;     /* 1.0f */
extern f32   lbl_8047BDA8;     /* evolution lerp speed */
extern f32   lbl_8047BDAC;     /* 0.0f */

/*
 * Functions in this translation unit (10 total):
 *
 * fn_800495C8  0x3E0  Evolution update (model interpolation, timing)
 * fn_800499BC  0xAC0  Evolution state machine (2752 bytes, calls fn_8004A7A8, fn_8004B598)
 * fn_8004A47C  0x32C  Evolution helper (no BSS)
 * fn_8004A7A8  0xAD0  Post-evolution processing (2768 bytes, stat recalc, move learn)
 * fn_8004B278  0x320  Evolution model transition
 * fn_8004B598  0x254  Evolution sequence sub-handler
 * fn_8004B7EC  0x5CC  Evolution completion (calls fn_8003C7C0, fn_80044630, fn_8004A47C)
 * fn_8004BDB8  0x034  Evolution result accessor A
 * fn_8004BDEC  0x010  Evolution result accessor B
 * fn_8004BDFC  0x010  Evolution result accessor C
 */


/* 0x800495C8 | size: 0x3E0 */
asm void fn_800495C8(void) { nofralloc
    #include "asm/GC6E01/nonmatching/evolution/fn_800495C8.s"
}

/* 0x800499BC | size: 0xAC0 */
asm void fn_800499BC(void) { nofralloc
    #include "asm/GC6E01/nonmatching/evolution/fn_800499BC.s"
}

