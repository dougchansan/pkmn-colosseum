/**
 * menuCB 0x80056C54 - 0x80057B34 with its own .sdata2 pool, 0x8047BEE0 -
 * 0x8047BF10. Retail repeats the 0x8047BEA0 constants (both conversion
 * biases, 2.0f, 1.0f, 0.5f, 255.0f, 0.0f, 0.8f) at 0x8047BEE0, so this range
 * was its own translation unit; it links as one, with every pool constant a
 * literal.
 */
#include "dolphin/types.h"

/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped function — see docs/RULE_EXCEPTIONS.md
 * Retail's pool opens with the u32 and then the s32 int-to-float biases ahead of
 * the float literals, although the first function to read them
 * (pcBoxDrawIconGrid) uses its float literals first. An unreferenced conversion
 * helper compiled ahead of it (and dead-stripped at link) creates the two
 * biases first, in that order; its body rests only on the pool layout. */
f32 menuCBStrippedConv(u32 a, s32 b) { return (f32)a + (f32)b; }

#define MENUCB_TU_80056C54_ONLY
#include "src/game/menu/menuCB_range_80055E38.c"
