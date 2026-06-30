#include "dolphin/types.h"

/*
 * Dolphin SDK debug interface (DB).
 * Handles low-level exception destination setup and debug printf.
 */

/* SDA-relative globals */
void* __DBInterface;
s32 DBVerbose;

/* External functions */
extern void OSReport(const char* fmt, ...);
extern void OSDumpContext(void* context);
extern void PPCHalt(void);

/* Forward declarations */
static void __DBExceptionDestinationAux(void);
asm void __DBExceptionDestination(void);

/*
 * DBInit - Initialize the debug interface.
 * Sets __DBInterface to 0x80000040, installs __DBExceptionDestination
 * into the exception table at 0x80000048, and enables verbose mode.
 */
void DBInit(void) {
    u32* exTable = (u32*)0x80000000;

    __DBInterface = (void*)0x80000040;

    /* Install exception destination handler at 0x48 */
    exTable[0x48 / 4] = (u32)__DBExceptionDestination + 0x80000000;

    DBVerbose = 1;
}

/*
 * __DBExceptionDestinationAux - Internal debug exception handler body.
 * Reports the exception and dumps context, then halts.
 */
static void __DBExceptionDestinationAux(void) {
    void* context;

    /* Read from physical address 0xC0 to get context pointer */
    context = (void*)((u32)(*(u32*)0xC0) + 0x80000000);

    OSReport("DBExceptionDestination\n");
    OSDumpContext(context);
    PPCHalt();
}

/*
 * __DBExceptionDestination - Exception vector handler entry point.
 * Enables FP and recoverable interrupt bits in MSR, then falls through
 * to the exception reporter. This is entered via the exception table.
 *
 * Must be written as asm because it manipulates MSR directly.
 */
asm void __DBExceptionDestination(void) {
    nofralloc
    mfmsr r3
    ori   r3, r3, 0x30
    mtmsr r3
    b     __DBExceptionDestinationAux
}

/*
 * __DBIsExceptionMarked - Check if a specific exception bit is marked.
 * Returns nonzero if the exception type is flagged in the interface.
 */
s32 __DBIsExceptionMarked(u8 exceptionType) {
    u32 mask;
    u32 flags;
    u32 idx = (u32)exceptionType & 0xFF;

    mask = 1 << idx;
    flags = ((u32*)__DBInterface)[1];

    return (s32)(flags & mask);
}

/*
 * DBPrintf - Debug printf (no-op in release builds).
 * The register spill pattern is a varargs no-op stub.
 */
asm void DBPrintf(const char* fmt, ...) {
    nofralloc
    stwu   r1, -0x70(r1)
    bne    cr1, @skip_fp
    stfd   f1, 0x28(r1)
    stfd   f2, 0x30(r1)
    stfd   f3, 0x38(r1)
    stfd   f4, 0x40(r1)
    stfd   f5, 0x48(r1)
    stfd   f6, 0x50(r1)
    stfd   f7, 0x58(r1)
    stfd   f8, 0x60(r1)
@skip_fp:
    stw    r3, 0x08(r1)
    stw    r4, 0x0c(r1)
    stw    r5, 0x10(r1)
    stw    r6, 0x14(r1)
    stw    r7, 0x18(r1)
    stw    r8, 0x1c(r1)
    stw    r9, 0x20(r1)
    stw    r10, 0x24(r1)
    addi   r1, r1, 0x70
    blr
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800A2C58 - 0x800A2C58 | size: 0x1C
 * __DBGetFirstCallback - Return the first callback in the debug interface list.
 * Returns NULL if the interface pointer is NULL.
 */
u32 fn_800A2C58(void) {
    if (__DBInterface == NULL) {
        return 0;
    }
    return *(u32*)__DBInterface;
}
