#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSContext.h"

/*
 * OSLink.c - OS module linkage helpers.
 *
 * Contains functions that sit between OSContext.c and OSError.c
 * in the link order. These handle OS context initialization
 * for the exception system and provide error-related helpers.
 *
 * Matches: 0x8009C1B0 - 0x8009C2E0
 *   fn_8009C1B4 (0x12C) - OSFillFPUContext or OSInitContext
 */

extern void OSReport(const char* fmt, ...);

/*
 * OSInitContext - Initialize a context for a new thread.
 * 0x8009C1B4 | size: 0x12C
 *
 * Sets up a context structure with the given program counter and
 * stack pointer. All GPRs are zeroed except r1 (SP) and r2 (RTOC).
 * SRR0 is set to the entry point, SRR1 is set with standard mode bits.
 */
void OSInitContext(OSContext* context, u32 pc, u32 sp) {
    int i;

    context->srr0 = pc;
    context->srr1 = 0x00008032;  /* MSR: EE=1, FP=1, ME=1, IR=1, DR=1 */
    context->gpr[1] = sp;

    /* Get current RTOC (r2) */
    {
        register u32 rtoc;
        asm { mr rtoc, r2 };
        context->gpr[2] = rtoc;
    }

    /* Zero all other GPRs */
    context->gpr[0] = 0;
    for (i = 3; i < 32; i++) {
        context->gpr[i] = 0;
    }

    /* Zero CR, LR, CTR, XER */
    context->cr  = 0;
    context->lr  = 0;
    context->ctr = 0;
    context->xer = 0;

    /* Zero GQRs */
    for (i = 0; i < 8; i++) {
        context->gqr[i] = 0;
    }

    /* Zero FPR/PSF saved state */
    context->mode  = 0;
    context->state = 0;

    /* Initialize FPSCR */
    context->fpscr_pad = 0;
}
