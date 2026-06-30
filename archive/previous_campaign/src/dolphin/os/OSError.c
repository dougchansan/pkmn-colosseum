#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"

/* va_list for PPC - MetroWerks CW inline */
typedef struct __va_list_struct {
    u8  gpr;
    u8  fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} __va_list_struct;
typedef __va_list_struct va_list[1];
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_end(ap)         ((void)0)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)

/*
 * OSError.c - Error handling and reporting.
 *
 * Provides OSReport (printf-like), OSSetErrorHandler, OSDumpContext,
 * and the unhandled exception handler.
 *
 * Matches: 0x8009C2E0 - 0x8009C860
 */

/* The error handler table: 17 entries indexed by error type */
OSErrorHandler __OSErrorTable[OS_ERROR_MAX];

extern int vprintf(const char* fmt, va_list args);

void OSReport(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

OSErrorHandler OSSetErrorHandler(u16 error, OSErrorHandler handler) {
    OSErrorHandler old;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    old = __OSErrorTable[error];
    __OSErrorTable[error] = handler;

    if (error == OS_ERROR_FPE) {
        u32 msr;
        u32 fpscr;

        msr = PPCMfmsr();
        PPCMtmsr(msr | 0x2000);     /* Enable FP in MSR */
        fpscr = PPCMffpscr();

        if (handler != NULL) {
            /* Enable FP exceptions for all threads */
            OSThread* thread;
            u32 fpeMask = 0x6006F8FF;

            for (thread = *(OSThread**)0x800000DC;
                 thread != NULL;
                 thread = *(OSThread**)((u8*)thread + 0x2FC))
            {
                thread->context.srr1 |= 0x900;

                if (!(thread->context.state & OS_CONTEXT_STATE_FPSAVED)) {
                    int j;
                    u32* ctx = (u32*)&thread->context;

                    thread->context.state |= OS_CONTEXT_STATE_FPSAVED;

                    /* Initialize FPR pairs to -1 */
                    for (j = 0; j < 4; j++) {
                        ctx[0x90/4 + j*16 + 0] = 0xFFFFFFFF;
                        ctx[0x90/4 + j*16 + 1] = 0xFFFFFFFF;
                        ctx[0x1C8/4 + j*16 + 0] = 0xFFFFFFFF;
                        ctx[0x1C8/4 + j*16 + 1] = 0xFFFFFFFF;
                        /* ... more pairs ... */
                    }

                    thread->context.fpscr_pad = 4;
                }

                {
                    u32 val = *(u32*)0x80478990;
                    u32 tmp;
                    val &= 0xF8;
                    tmp = *(u32*)((u8*)thread + 0x194);
                    tmp |= val;
                    *(u32*)((u8*)thread + 0x194) = tmp;
                    tmp &= fpeMask;
                    *(u32*)((u8*)thread + 0x194) = tmp;
                }
            }

            {
                u32 val = *(u32*)0x80478990;
                msr |= 0x900;
                val &= 0xF8;
                fpscr |= val;
            }
        } else {
            /* Disable FP exceptions for all threads */
            OSThread* thread;
            u32 fpeMask = 0x6006F8FF;

            for (thread = *(OSThread**)0x800000DC;
                 thread != NULL;
                 thread = *(OSThread**)((u8*)thread + 0x2FC))
            {
                thread->context.srr1 &= ~0x900;
                {
                    u32 tmp = *(u32*)((u8*)thread + 0x194);
                    tmp &= ~0xF8;
                    *(u32*)((u8*)thread + 0x194) = tmp;
                    tmp &= fpeMask;
                    *(u32*)((u8*)thread + 0x194) = tmp;
                }
            }

            fpscr &= ~0xF8;
            msr &= ~0x900;
        }

        fpscr &= 0x6006F8FF;
        PPCMtfpscr(fpscr);
        PPCMtmsr(msr);
    }

    OSRestoreInterrupts(enabled);
    return old;
}

void __OSUnhandledException(u8 exception, OSContext* context, u32 dsisr, u32 dar) {
    s64 now;
    u32 msr;

    now = OSGetTime();

    msr = context->srr1;

    if (exception == OS_EXCEPTION_PROGRAM &&
        (msr & 0x00020000) &&
        __OSErrorTable[OS_ERROR_FPE] != NULL)
    {
        __OSErrorTable[OS_ERROR_FPE](OS_ERROR_FPE, context, dsisr, dar);
        OSLoadContext(context);
        return;
    }

    if (exception == OS_EXCEPTION_DSI &&
        __OSErrorTable[OS_ERROR_PROTECTION] != NULL)
    {
        /* Check for memory protection fault */
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, dsisr, dar);
        OSLoadContext(context);
        return;
    }

    OSReport("Unhandled Exception %d\n", exception);
    OSReport("\n");
    OSReport("  GPR00 %08X GPR08 %08X GPR16 %08X GPR24 %08X\n",
             context->gpr[0], context->gpr[8], context->gpr[16], context->gpr[24]);
    OSReport("  GPR01 %08X GPR09 %08X GPR17 %08X GPR25 %08X\n",
             context->gpr[1], context->gpr[9], context->gpr[17], context->gpr[25]);
    OSReport("  GPR02 %08X GPR10 %08X GPR18 %08X GPR26 %08X\n",
             context->gpr[2], context->gpr[10], context->gpr[18], context->gpr[26]);
    OSReport("  GPR03 %08X GPR11 %08X GPR19 %08X GPR27 %08X\n",
             context->gpr[3], context->gpr[11], context->gpr[19], context->gpr[27]);
    OSReport("  GPR04 %08X GPR12 %08X GPR20 %08X GPR28 %08X\n",
             context->gpr[4], context->gpr[12], context->gpr[20], context->gpr[28]);
    OSReport("  GPR05 %08X GPR13 %08X GPR21 %08X GPR29 %08X\n",
             context->gpr[5], context->gpr[13], context->gpr[21], context->gpr[29]);
    OSReport("  GPR06 %08X GPR14 %08X GPR22 %08X GPR30 %08X\n",
             context->gpr[6], context->gpr[14], context->gpr[22], context->gpr[30]);
    OSReport("  GPR07 %08X GPR15 %08X GPR23 %08X GPR31 %08X\n",
             context->gpr[7], context->gpr[15], context->gpr[23], context->gpr[31]);
    OSReport("  LR  = %08X  SRR0 = %08X  SRR1 = %08X\n",
             context->lr, context->srr0, context->srr1);
    OSReport("  CR  = %08X  XER  = %08X  CTR  = %08X\n",
             context->cr, context->xer, context->ctr);
    OSReport("  DSISR = %08X  DAR  = %08X\n", dsisr, dar);

    OSReport("\nInstruction at SRR0: ");
    if (context->srr0 != 0 && !(context->srr0 & 3)) {
        OSReport("0x%08X\n", *(u32*)context->srr0);
    } else {
        OSReport("(invalid address)\n");
    }

    OSDumpContext(context);
    PPCHalt();
}
