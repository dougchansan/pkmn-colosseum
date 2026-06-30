#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * mwtrace.c - CRT / MetroTRK trace support.
 *
 * Interrupt-guard helpers used by the trace runtime: a small object holds the
 * saved interrupt state, enter() disables interrupts and stashes the prior
 * level, leave() restores it.
 */

typedef struct TRKLock {
    /* 0x00 */ BOOL level;
} TRKLock;

/* fn_800C4548 - 0x800C4548 | size: 0x24 */
void fn_800C4548(TRKLock *lock) {
    OSRestoreInterrupts(lock->level);
}

/* fn_800C456C - 0x800C456C | size: 0x30 */
void fn_800C456C(TRKLock *lock) {
    lock->level = OSDisableInterrupts();
}

/* fn_800C459C - 0x800C459C | size: 0x4 */
void fn_800C459C(void) {
}
