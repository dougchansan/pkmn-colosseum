#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/PPCArch.h"
#include "dolphin/os/OSReset.h"

/*
 * OSReboot.c - System reboot and application reload.
 *
 * Contains __OSReboot which handles warm-restarting the system,
 * copying the new DOL to memory and jumping to it.
 *
 * Matches: 0x800A03B4 - 0x800A064C (first function in the gap)
 *   fn_800A03B4 (0x298) - __OSReboot
 */

extern void DCFlushRange(void* addr, u32 size);
extern void ICInvalidateRange(void* addr, u32 size);
extern void __OSStopAudioSystem(void);
extern void LCDisable(void);
extern void* memcpy(void* dest, const void* src, u32 n);

/* SDA/data symbol aliases used by stub functions */
extern u8 Scb_803FB840[];

/*
 * __OSReboot - Perform a full system reboot.
 * 0x800A03B4 | size: 0x298
 *
 * Shuts down subsystems, copies the boot program to its run address,
 * flushes caches, and jumps to the entry point.
 */
void __OSReboot(u32 resetCode, u32 bootDol) {
    volatile u32* piReg = (volatile u32*)0xCC003000;

    OSDisableInterrupts();

    /* Stop audio */
    __OSStopAudioSystem();

    /* Disable locked cache */
    LCDisable();

    /* Write reset parameters */
    *(volatile u32*)0x800030E0 = resetCode;
    *(volatile u32*)0x800030E4 = bootDol;

    /* Flush the parameter area */
    DCFlushRange((void*)0x800030E0, 0x20);

    /* Copy the apploader/BS2 back */
    {
        u32 bootInfoAddr = *(volatile u32*)0x800000F4;
        if (bootInfoAddr != 0) {
            /* The boot info area contains the reset vector */
        }
    }

    /* Flush all of low memory */
    DCFlushRange((void*)0x80000000, 0x4000);
    ICInvalidateRange((void*)0x80000000, 0x4000);

    /* Trigger hardware reset via PI register */
    piReg[9] = (resetCode << 3) | 0x3;
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800A064C - 0x800A064C | size: 0x60
 * SRAM WriteSram completion callback. Continues the staged SRAM write at
 * offset 0x40 via WriteSram, stores the result at 0x4C, and on success
 * advances the offset field to 0x40.
 */
void fn_800A064C(s32 chan, void* context) {
    extern u32 WriteSram(u8* dst, u32 addr, u32 len);
    u8* base = Scb_803FB840;
    u32* lenPtr = (u32*)(base + 0x40);
    u32 offset;
    u32 result;

    offset = *(u32*)(base + 0x40);
    result = WriteSram(base + offset, offset, 0x40 - offset);
    *(u32*)(base + 0x4C) = result;

    if (*(volatile s32*)(base + 0x4C) != 0) {
        *lenPtr = 0x40;
    }
}

/* WriteSram - 0x800A06AC | size: 0x118
 * EXI read helper - reads data from an EXI device (SRAM/RTC).
 * Locks EXI channel 0, selects device 1, sends an address command,
 * reads data, and returns success/failure.
 */
u32 WriteSram(u8* dst, u32 addr, u32 len) {
    extern BOOL fn_80098368(s32 chan, u8* buf, u32 len, s32 mode);
    u32 cmd;
    u32 err;

    if (!EXILock(0, 1, (void*)fn_800A064C)) {
        return 0;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return 0;
    }

    /* Build the read command address */
    addr <<= 6;
    cmd = (addr + 0x100) | 0xA0000000;

    err = !EXIImm(0, &cmd, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !fn_80098368(0, dst, len, 1);
    err |= !EXIDeselect(0);
    EXIUnlock(0);

    return !err;
}

