#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_802EE06C[];

extern u8 menuColoBattleC0Ctrl[];

/* Carved .data unit 0x802EE06C..0x802EE0F0: menuColoBattleC0Ctrl's switch table. It
 * starts 4-aligned, so the compiled table cannot link from its own unit. */
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
