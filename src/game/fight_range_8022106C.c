/**
 * @file fight_range_8022106C.c
 * @brief Exact pure-C fight-sequence helper, 0x8022106C - 0x80221104.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;

/*
 * wsCmd47 (0x8022106C)
 * Sequence opcode: maps the high nibble of the field byte at +0x1601e to a
 * base offset (0x10 -> 0xf, 0x20 -> 0x27, 0x90 -> 0x16, 0xa0 -> 0x2e), adds
 * the low nibble minus one into +0x160a4, clears +0x160a5 and advances the
 * sequence PC by one byte.
 */
void wsCmd47(void* ctx, u32 param1, u32 param2) {
    extern u8 lbl_80379F58[];
    u8 code;
    u8 offset;

    offset = 0;
    code = lbl_80379F58[0x1601e];
    switch (code & 0xf0) {
        case 0x10: offset = 0xf; break;
        case 0x20: offset = 0x27; break;
        case 0x90: offset = 0x16; break;
        case 0xa0: offset = 0x2e; break;
        default: break;
    }
    lbl_80379F58[0x160a4] = offset + (code & 0xf) - 1;
    lbl_80379F58[0x160a5] = 0;
    lbl_8047B610++;
}
