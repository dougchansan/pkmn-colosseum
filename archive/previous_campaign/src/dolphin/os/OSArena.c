#include "dolphin/os/OS.h"

/*
 * OSArena.c - Arena memory management.
 *
 * The arena is the region of memory between ArenaLo and ArenaHi
 * that the application can freely use for dynamic allocations.
 *
 * Matches: 0x8009AFB0 - 0x8009AFCC
 */

static void* __OSArenaHi;
static void* __OSArenaLo;

void* OSGetArenaHi(void) {
    return __OSArenaHi;
}

void* OSGetArenaLo(void) {
    return __OSArenaLo;
}

void OSSetArenaHi(void* addr) {
    __OSArenaHi = addr;
}

void OSSetArenaLo(void* addr) {
    __OSArenaLo = addr;
}
