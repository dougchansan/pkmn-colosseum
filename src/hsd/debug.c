/**
 * @file debug.c
 * @brief HAL sysdolphin debug.c: HSD_SaveContext, HSD_Panic and __assert,
 *        0x80196CE0-0x80196EB4.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/debug.c) and checked against Colosseum's retail
 * code. The newer sysdolphin saves the panic context with its own
 * HSD_SaveContext (a hand-written copy of OSSaveContext that also stores
 * the GQRs and chains to OSFillFPUContext) instead of the SDK's
 * OSSaveContext, and __assert inlines HSD_Panic. The library is built with
 * deferred inlining, so MWCC emits the C functions in reverse source order.
 *
 * HSD_SaveContext (0x80196CE0) is assembly in HAL's source; it is kept as
 * registered library asm (docs/asm_evidence/hsd_debug.md). The panic context
 * (lbl_80465080) stays a global because HSD_SaveContext stores into it.
 */
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/types.h"
#include "sysdolphin/baselib/debug.h"

typedef void (*ReportCallback)(const char* str, u32 len);
/* Retail calls the panic callback with cr1 cleared, as for a variadic
 * function, so its type takes a variable argument list. */
typedef void (*PanicCallback)(OSContext* context, ...);

/* OSPanic */
void errorPanic(const char* file, s32 line, const char* msg, ...);
/* Saves into lbl_80465080 itself. */
void HSD_SaveContext(void);
/* OSContext.c */
extern void OSFillFPUContext(OSContext* context);

/* HAL's panic context. */
OSContext lbl_80465080;

static ReportCallback reportCallback;
static PanicCallback panicCallback;

/* Hand-written HAL sysdolphin asm; evidence: docs/asm_evidence/hsd_debug.md.
 * A copy of OSSaveContext that saves into the panic context and also stores
 * the GQRs, then tail-branches to OSFillFPUContext (a declared branch
 * target) to save the FPU state. */
asm void HSD_SaveContext(void) {
    nofralloc

    mtsprg  0, r3
    lis     r3, lbl_80465080@ha
    addi    r3, r3, lbl_80465080@l
    stmw    r0, 0x0(r3)
    mfsprg  r4, 0
    stw     r4, 0xC(r3)
    mfspr   r4, GQR0
    stw     r4, 0x1A4(r3)
    mfspr   r4, GQR1
    stw     r4, 0x1A8(r3)
    mfspr   r4, GQR2
    stw     r4, 0x1AC(r3)
    mfspr   r4, GQR3
    stw     r4, 0x1B0(r3)
    mfspr   r4, GQR4
    stw     r4, 0x1B4(r3)
    mfspr   r4, GQR5
    stw     r4, 0x1B8(r3)
    mfspr   r4, GQR6
    stw     r4, 0x1BC(r3)
    mfspr   r4, GQR7
    stw     r4, 0x1C0(r3)
    mfcr    r4
    stw     r4, 0x80(r3)
    mflr    r4
    stw     r4, 0x84(r3)
    mfctr   r4
    stw     r4, 0x88(r3)
    mfxer   r4
    stw     r4, 0x8C(r3)
    mfsrr0  r4
    stw     r4, 0x198(r3)
    mfsrr1  r4
    stw     r4, 0x19C(r3)
    lhz     r4, 0x1A2(r3)
    ori     r4, r4, 0x1
    sth     r4, 0x1A2(r3)
    b       OSFillFPUContext
}

void __assert(const char* file, u32 line, const char* expr)
{
    OSReport("assertion \"%s\" failed", expr);
    HSD_Panic(file, line, "");
}

void HSD_Panic(const char* file, u32 line, const char* msg)
{
    if (panicCallback != NULL) {
        HSD_SaveContext();
        OSReport("%s in %s on line %d.\n", msg, file, line);
        panicCallback(&lbl_80465080);
    }
    errorPanic(file, line, msg);
}

void HSD_SetReportCallback(ReportCallback cb)
{
    reportCallback = cb;
}

void HSD_SetPanicCallback(PanicCallback cb)
{
    panicCallback = cb;
}
