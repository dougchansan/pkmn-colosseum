#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_802EE06C[];

extern void menuColoBattleC0Ctrl(void* menu);

/* RULE-EXCEPTION(user-approved): named stand-in for another function's
 * compiler switch table - see docs/RULE_EXCEPTIONS.md.
 * This definition is included after menu_middle_r59_8006CCC0_o1.c's generated
 * tables. Its 0x802EE06C..0x802EE0F0 span starts four-byte aligned at .data+0xBC,
 * so it shares that source owner instead of introducing an eight-byte-aligned
 * standalone section. menuColoBattleC0Ctrl imports this table after exact comparison. */
void* jumptable_802EE06C[33] = {
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x88),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x90),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x98),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xA0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xA8),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xB0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xB8),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0xC0),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x80),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x88),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x88),
    (void*)((u8*)menuColoBattleC0Ctrl + 0x88),
};
