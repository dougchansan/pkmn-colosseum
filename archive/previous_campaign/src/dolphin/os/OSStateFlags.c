#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSCache.h"

/*
 * OSStateFlags.c - OS state flags and reboot parameter management.
 *
 * Manages persistent state flags stored in a reserved memory region
 * that survives warm resets. Used for passing data across reboots,
 * app-switch parameters, and NAND state tracking.
 *
 * Matches: 0x8009F77C - 0x8009FAF8
 *   fn_8009F77C (0x38) - __OSGetStateFlags
 *   fn_8009F7B4 (0xDC) - __OSSetStateFlags
 *   fn_8009F890 (0xC8) - __OSGetDiscState
 *   fn_8009F958 (0x70) - __OSSetDiscState
 *   fn_8009F9C8 (0x20) - __OSGetBootMode
 *   fn_8009F9E8 (0xD4) - __OSSetBootMode
 *   fn_8009FABC (0x20) - __OSGetAppType
 *   fn_8009FADC (0x10) - __OSSetAppType
 *   fn_8009FAEC (0x0C) - (helper/getter)
 */

/* State flags memory location - persistent across resets */
#define OS_STATE_FLAGS_ADDR  ((volatile u32*)0x800030C0)

typedef struct OSStateFlags {
    u32 checksum;       /* 0x00 */
    u32 flags;          /* 0x04 */
    u32 discState;      /* 0x08 */
    u32 bootMode;       /* 0x0C */
    u32 appType;        /* 0x10 */
    u32 reserved[3];    /* 0x14 */
} OSStateFlags;

static OSStateFlags StateFlags;

/*
 * __OSGetStateFlags - Read the current state flags.
 * 0x8009F77C | size: 0x38
 */
u32 __OSGetStateFlags(void) {
    return StateFlags.flags;
}

/*
 * __OSSetStateFlags - Write state flags and flush to memory.
 * 0x8009F7B4 | size: 0xDC
 */
void __OSSetStateFlags(u32 flags) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    StateFlags.flags = flags;

    /* Calculate checksum */
    {
        u32* data = (u32*)&StateFlags.flags;
        u32 sum = 0;
        u32 i;
        for (i = 0; i < (sizeof(OSStateFlags) - 4) / 4; i++) {
            sum += data[i];
        }
        StateFlags.checksum = sum;
    }

    /* Write to persistent memory */
    {
        volatile u32* dest = OS_STATE_FLAGS_ADDR;
        u32* src = (u32*)&StateFlags;
        u32 i;
        for (i = 0; i < sizeof(OSStateFlags) / 4; i++) {
            dest[i] = src[i];
        }
    }

    DCFlushRange((void*)OS_STATE_FLAGS_ADDR, sizeof(OSStateFlags));
    OSRestoreInterrupts(enabled);
}

/*
 * __OSGetDiscState - Get the current disc state.
 * 0x8009F890 | size: 0xC8
 */
u32 __OSGetDiscState(void) {
    return StateFlags.discState;
}

/*
 * __OSSetDiscState - Set the disc state.
 * 0x8009F958 | size: 0x70
 */
void __OSSetDiscState(u32 state) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    StateFlags.discState = state;
    __OSSetStateFlags(StateFlags.flags);
    OSRestoreInterrupts(enabled);
}

/*
 * __OSGetBootMode - Get the boot mode.
 * 0x8009F9C8 | size: 0x20
 */
u32 __OSGetBootMode(void) {
    return StateFlags.bootMode;
}

/*
 * __OSSetBootMode - Set the boot mode.
 * 0x8009F9E8 | size: 0xD4
 */
void __OSSetBootMode(u32 mode) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    StateFlags.bootMode = mode;
    __OSSetStateFlags(StateFlags.flags);
    OSRestoreInterrupts(enabled);
}

/*
 * __OSGetAppType - Get the application type.
 * 0x8009FABC | size: 0x20
 */
u32 __OSGetAppType(void) {
    return StateFlags.appType;
}

/*
 * __OSSetAppType - Set the application type.
 * 0x8009FADC | size: 0x10
 */
void __OSSetAppType(u32 type) {
    StateFlags.appType = type;
}

/*
 * fn_8009FAEC - Helper getter.
 * 0x8009FAEC | size: 0x0C
 */
u32 __OSGetResetCodeLo(void) {
    return *(volatile u32*)0x800030E4;
}
