#include "dolphin/types.h"

#pragma section ".data"

extern u8 pad_05_802EDE54_data[];
extern u8 lbl_802EDE58[];
extern void* jumptable_802EE0F0[];
extern void* jumptable_802EE20C[];
extern void* jumptable_802EE31C[];

extern u8 menuColoBattleBEDrawSprite[];
extern u8 menuColoBattleBEDrawSprite2[];
extern u8 menuColoBattleBEDrawSprite3[];

/* Auto-carved .data unit 0x802EDE54..0x802EDE78 (2 objects). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

u8 pad_05_802EDE54_data[4] = {
    0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802EDE58[32] = {
    0x00, 0x00, 0x3B, 0xEB, 0x00, 0x00, 0x3B, 0xF0, 0x00, 0x00, 0x3B, 0xF3,
    0x00, 0x00, 0x3B, 0xF5, 0x00, 0x00, 0x3B, 0xF7, 0x00, 0x00, 0x3B, 0xF8,
    0x00, 0x00, 0x41, 0xFD, 0x00, 0x00, 0x3B, 0xF9,
};

/* 0x802EE0F0..0x802EE454 (jumptable_802EE0F0, jumptable_802EE20C and
 * jumptable_802EE31C) is game/menu/menu_middle_r47_prefix.c's own .data. */
