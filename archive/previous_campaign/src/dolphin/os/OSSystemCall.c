#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"

/*
 * OSSystemCall.c - System call vector installation.
 *
 * Installs the system call exception vector (exception 8) at the
 * appropriate address in the exception vector table. The vector
 * handles the PowerPC 'sc' instruction.
 *
 * Adapted from doldecomp/melee matching implementation.
 *
 * Matches: 0x800A1208 - 0x800A128C
 *   __OSSystemCallVectorStart (0x20) - The actual vector code
 *   __OSInitSystemCall (0x64)        - Installs the vector
 *   fn_800A128C (0x04)               - (nop/pad)
 */

extern void ICInvalidateRange(void* addr, u32 size);
extern void DCFlushRangeNoSync(void* addr, u32 size);
extern void* memcpy(void* dest, const void* src, u32 n);

/*
 * System call vector: a small assembly stub that is copied to
 * exception address 0x80000C00.
 */
extern void __OSSystemCallVectorStart(void);
extern void __OSSystemCallVectorEnd(void);

/*
 * __OSSystemCallVector - The system call handler stub.
 * 0x800A1208 | size: 0x20
 *
 * This is the code that gets copied to the exception vector location.
 * On a 'sc' instruction, the CPU jumps here.
 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __OSSystemCallVector(void) {
    nofralloc
entry __OSSystemCallVectorStart
    /* System call vector: just return from interrupt */
    mfmsr   r10
    ori     r10, r10, 0x0002  /* Set RI bit */
    mtmsr   r10

    /* Load the OS_CURRENT_CONTEXT into r10 */
    mfsrr0  r10
    rfi
entry __OSSystemCallVectorEnd
}
#pragma pop

/*
 * __OSInitSystemCall - Install the system call exception vector.
 * 0x800A1228 | size: 0x64
 *
 * Copies the system call vector stub to the exception table address
 * 0x80000C00 (exception 8 * 0x100).
 */
void __OSInitSystemCall(void) {
    void* dest;
    u32 size;

    dest = (void*)0x80000C00;
    size = (u32)__OSSystemCallVectorEnd - (u32)__OSSystemCallVectorStart;

    memcpy(dest, __OSSystemCallVectorStart, size);

    DCFlushRangeNoSync(dest, 0x100);
    ICInvalidateRange(dest, 0x100);
}
