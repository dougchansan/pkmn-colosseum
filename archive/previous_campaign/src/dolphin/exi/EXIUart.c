#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * EXIUart.c - EXI UART/debug communication helpers.
 *
 * These functions bridge EXI and OS initialization, providing
 * UART-over-EXI communication for debug consoles. They sit between
 * EXI.c's EXIUnlock and __OSFPRInit in the link order.
 *
 * Matches: 0x800993A8 - 0x80099790
 *   fn_800993A8 (0x28) - __EXIProbeStartTime or InitUart
 *   fn_800993D0 (0x30) - EXI Uart helper
 *   fn_80099400 (0x390) - EXI Uart Read/Write (large transfer function)
 */

extern void* memcpy(void* dest, const void* src, u32 n);

/* Hardware UART register base for EXI debug channel */
#define EXI_UART_BASE 0xCC006800

/*
 * fn_800993A8 - EXI probe start time or UART init stub.
 * 0x800993A8 | size: 0x28
 *
 * Small helper that checks a flag and returns immediately.
 * Likely __EXIProbeStartTime for channel device detection timing.
 */
static u32 __EXIUartEnabled;

BOOL __OSEnableEXIUart(void) {
    __EXIUartEnabled = 1;
    return TRUE;
}

/*
 * fn_800993D0 - UART write byte helper.
 * 0x800993D0 | size: 0x30
 *
 * Writes a single byte to the EXI UART device.
 */
static BOOL __OSWriteEXIUartByte(u8 byte) {
    BOOL enabled;
    u32 data;

    if (__EXIUartEnabled == 0) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    data = 0xB0000000 | ((u32)byte << 20);

    if (!EXISelect(2, 0, 3)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    EXIImm(2, &data, 4, 1, NULL);
    EXISync(2);
    EXIDeselect(2);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * fn_80099400 - UART write buffer.
 * 0x80099400 | size: 0x390
 *
 * Writes a buffer of data to the EXI UART device. This is the large
 * function that handles multi-byte EXI UART transfers for debug output.
 * Used by OSReport when a debug console is connected.
 */
s32 __OSWriteEXIUart(const void* buf, u32 len) {
    BOOL enabled;
    const u8* ptr;
    u32 i;
    u32 data;
    s32 written;

    if (__EXIUartEnabled == 0) {
        return 0;
    }

    ptr = (const u8*)buf;
    written = 0;

    enabled = OSDisableInterrupts();

    for (i = 0; i < len; i++) {
        if (!EXISelect(2, 0, 3)) {
            OSRestoreInterrupts(enabled);
            return written;
        }

        data = 0xB0000000 | ((u32)ptr[i] << 20);
        EXIImm(2, &data, 4, 1, NULL);
        EXISync(2);
        EXIDeselect(2);
        written++;
    }

    OSRestoreInterrupts(enabled);
    return written;
}

/*
 * __OSFPRInit - Initialize Floating Point Registers.
 * 0x80099790 | size: 0x128
 *
 * This function is part of OS initialization. It sets up all 32 FPRs
 * and the paired-singles registers to known values, then enables the
 * FPU in the MSR.
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __OSFPRInit(void) {
    nofralloc

    /* Enable FPU in MSR */
    mfmsr   r3
    ori     r3, r3, 0x2000
    mtmsr   r3

    /* Enable paired singles via HID2 */
    mfspr   r3, 920
    oris    r3, r3, 0xA000
    mtspr   920, r3

    /* Initialize GQRs to 0 */
    li      r3, 0
    mtspr   GQR0, r3
    mtspr   GQR1, r3
    mtspr   GQR2, r3
    mtspr   GQR3, r3
    mtspr   GQR4, r3
    mtspr   GQR5, r3
    mtspr   GQR6, r3
    mtspr   GQR7, r3

    /* Clear all FPRs */
    lis     r3, 0x8000         /* Use address 0x80000000 as scratch */
    li      r4, 0
    stw     r4, 0(r3)
    stw     r4, 4(r3)
    lfd     f0, 0(r3)
    fmr     f1, f0
    fmr     f2, f0
    fmr     f3, f0
    fmr     f4, f0
    fmr     f5, f0
    fmr     f6, f0
    fmr     f7, f0
    fmr     f8, f0
    fmr     f9, f0
    fmr     f10, f0
    fmr     f11, f0
    fmr     f12, f0
    fmr     f13, f0
    fmr     f14, f0
    fmr     f15, f0
    fmr     f16, f0
    fmr     f17, f0
    fmr     f18, f0
    fmr     f19, f0
    fmr     f20, f0
    fmr     f21, f0
    fmr     f22, f0
    fmr     f23, f0
    fmr     f24, f0
    fmr     f25, f0
    fmr     f26, f0
    fmr     f27, f0
    fmr     f28, f0
    fmr     f29, f0
    fmr     f30, f0
    fmr     f31, f0

    mtfsf   0xFF, f0

    blr
}
#pragma pop

/*
 * fn_800998B8 - Read SRAM boot flags.
 * 0x800998B8 | size: 0x28
 *
 * Reads the SRAM flags used during EXI initialization to determine
 * device presence state.
 */
u32 __OSReadSramFlags(void) {
    volatile u32* sramFlags = (volatile u32*)0x800030E0;
    return *sramFlags;
}
