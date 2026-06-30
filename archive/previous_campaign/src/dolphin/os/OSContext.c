#include "dolphin/os/OSContext.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/PPCArch.h"
#include "dolphin/db/DB.h"

/*
 * OSContext.c - CPU context save/restore and FPU context management.
 *
 * Provides functions for saving and restoring the full CPU context,
 * including general-purpose registers, special-purpose registers,
 * floating-point registers, and paired-singles.
 *
 * Matches: 0x8009B914 - 0x8009C1B0
 */

/* Hardware memory locations */
#define OS_CURRENTCONTEXT_PADDR (*(volatile u32*)0x800000C0)
#define OS_CURRENTCONTEXT       (*(OSContext* volatile*)0x800000D4)
#define OS_FPUCONTEXT           (*(OSContext* volatile*)0x800000D8)

extern void OSSwitchFPUContext(u8 exception, OSContext* context);

/* __OSLoadFPUContext - loads FPU registers from context */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __OSLoadFPUContext(register u8 unused, register OSContext* context) {
    nofralloc
    lhz     r5, 0x01A2(r4)
    clrlwi. r5, r5, 31
    beq     _exit_load

    lfd     f0, 0x0190(r4)
    mtfsf   255, f0

    mfspr   r5, 920
    extrwi. r5, r5, 1, 2
    beq     _load_fpr

    /* load paired-singles */
    psq_l   f0,  0x01C8(r4), 0, 0
    psq_l   f1,  0x01D0(r4), 0, 0
    psq_l   f2,  0x01D8(r4), 0, 0
    psq_l   f3,  0x01E0(r4), 0, 0
    psq_l   f4,  0x01E8(r4), 0, 0
    psq_l   f5,  0x01F0(r4), 0, 0
    psq_l   f6,  0x01F8(r4), 0, 0
    psq_l   f7,  0x0200(r4), 0, 0
    psq_l   f8,  0x0208(r4), 0, 0
    psq_l   f9,  0x0210(r4), 0, 0
    psq_l   f10, 0x0218(r4), 0, 0
    psq_l   f11, 0x0220(r4), 0, 0
    psq_l   f12, 0x0228(r4), 0, 0
    psq_l   f13, 0x0230(r4), 0, 0
    psq_l   f14, 0x0238(r4), 0, 0
    psq_l   f15, 0x0240(r4), 0, 0
    psq_l   f16, 0x0248(r4), 0, 0
    psq_l   f17, 0x0250(r4), 0, 0
    psq_l   f18, 0x0258(r4), 0, 0
    psq_l   f19, 0x0260(r4), 0, 0
    psq_l   f20, 0x0268(r4), 0, 0
    psq_l   f21, 0x0270(r4), 0, 0
    psq_l   f22, 0x0278(r4), 0, 0
    psq_l   f23, 0x0280(r4), 0, 0
    psq_l   f24, 0x0288(r4), 0, 0
    psq_l   f25, 0x0290(r4), 0, 0
    psq_l   f26, 0x0298(r4), 0, 0
    psq_l   f27, 0x02A0(r4), 0, 0
    psq_l   f28, 0x02A8(r4), 0, 0
    psq_l   f29, 0x02B0(r4), 0, 0
    psq_l   f30, 0x02B8(r4), 0, 0
    psq_l   f31, 0x02C0(r4), 0, 0

_load_fpr:
    lfd     f0,  0x0090(r4)
    lfd     f1,  0x0098(r4)
    lfd     f2,  0x00A0(r4)
    lfd     f3,  0x00A8(r4)
    lfd     f4,  0x00B0(r4)
    lfd     f5,  0x00B8(r4)
    lfd     f6,  0x00C0(r4)
    lfd     f7,  0x00C8(r4)
    lfd     f8,  0x00D0(r4)
    lfd     f9,  0x00D8(r4)
    lfd     f10, 0x00E0(r4)
    lfd     f11, 0x00E8(r4)
    lfd     f12, 0x00F0(r4)
    lfd     f13, 0x00F8(r4)
    lfd     f14, 0x0100(r4)
    lfd     f15, 0x0108(r4)
    lfd     f16, 0x0110(r4)
    lfd     f17, 0x0118(r4)
    lfd     f18, 0x0120(r4)
    lfd     f19, 0x0128(r4)
    lfd     f20, 0x0130(r4)
    lfd     f21, 0x0138(r4)
    lfd     f22, 0x0140(r4)
    lfd     f23, 0x0148(r4)
    lfd     f24, 0x0150(r4)
    lfd     f25, 0x0158(r4)
    lfd     f26, 0x0160(r4)
    lfd     f27, 0x0168(r4)
    lfd     f28, 0x0170(r4)
    lfd     f29, 0x0178(r4)
    lfd     f30, 0x0180(r4)
    lfd     f31, 0x0188(r4)

_exit_load:
    blr
}
#pragma pop

/* __OSSaveFPUContext - saves FPU registers to context */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __OSSaveFPUContext(register u8 unused1, register u8 unused2, register OSContext* context) {
    nofralloc
    lhz     r3, 0x01A2(r5)
    ori     r3, r3, OS_CONTEXT_STATE_FPSAVED
    sth     r3, 0x01A2(r5)

    stfd    f0,  0x0090(r5)
    stfd    f1,  0x0098(r5)
    stfd    f2,  0x00A0(r5)
    stfd    f3,  0x00A8(r5)
    stfd    f4,  0x00B0(r5)
    stfd    f5,  0x00B8(r5)
    stfd    f6,  0x00C0(r5)
    stfd    f7,  0x00C8(r5)
    stfd    f8,  0x00D0(r5)
    stfd    f9,  0x00D8(r5)
    stfd    f10, 0x00E0(r5)
    stfd    f11, 0x00E8(r5)
    stfd    f12, 0x00F0(r5)
    stfd    f13, 0x00F8(r5)
    stfd    f14, 0x0100(r5)
    stfd    f15, 0x0108(r5)
    stfd    f16, 0x0110(r5)
    stfd    f17, 0x0118(r5)
    stfd    f18, 0x0120(r5)
    stfd    f19, 0x0128(r5)
    stfd    f20, 0x0130(r5)
    stfd    f21, 0x0138(r5)
    stfd    f22, 0x0140(r5)
    stfd    f23, 0x0148(r5)
    stfd    f24, 0x0150(r5)
    stfd    f25, 0x0158(r5)
    stfd    f26, 0x0160(r5)
    stfd    f27, 0x0168(r5)
    stfd    f28, 0x0170(r5)
    stfd    f29, 0x0178(r5)
    stfd    f30, 0x0180(r5)
    stfd    f31, 0x0188(r5)

    mffs    f0
    stfd    f0, 0x0190(r5)
    lfd     f0, 0x0090(r5)

    mfspr   r3, 920
    extrwi. r3, r3, 1, 2
    beq     _exit_save

    /* save paired-singles */
    psq_st  f0,  0x01C8(r5), 0, 0
    psq_st  f1,  0x01D0(r5), 0, 0
    psq_st  f2,  0x01D8(r5), 0, 0
    psq_st  f3,  0x01E0(r5), 0, 0
    psq_st  f4,  0x01E8(r5), 0, 0
    psq_st  f5,  0x01F0(r5), 0, 0
    psq_st  f6,  0x01F8(r5), 0, 0
    psq_st  f7,  0x0200(r5), 0, 0
    psq_st  f8,  0x0208(r5), 0, 0
    psq_st  f9,  0x0210(r5), 0, 0
    psq_st  f10, 0x0218(r5), 0, 0
    psq_st  f11, 0x0220(r5), 0, 0
    psq_st  f12, 0x0228(r5), 0, 0
    psq_st  f13, 0x0230(r5), 0, 0
    psq_st  f14, 0x0238(r5), 0, 0
    psq_st  f15, 0x0240(r5), 0, 0
    psq_st  f16, 0x0248(r5), 0, 0
    psq_st  f17, 0x0250(r5), 0, 0
    psq_st  f18, 0x0258(r5), 0, 0
    psq_st  f19, 0x0260(r5), 0, 0
    psq_st  f20, 0x0268(r5), 0, 0
    psq_st  f21, 0x0270(r5), 0, 0
    psq_st  f22, 0x0278(r5), 0, 0
    psq_st  f23, 0x0280(r5), 0, 0
    psq_st  f24, 0x0288(r5), 0, 0
    psq_st  f25, 0x0290(r5), 0, 0
    psq_st  f26, 0x0298(r5), 0, 0
    psq_st  f27, 0x02A0(r5), 0, 0
    psq_st  f28, 0x02A8(r5), 0, 0
    psq_st  f29, 0x02B0(r5), 0, 0
    psq_st  f30, 0x02B8(r5), 0, 0
    psq_st  f31, 0x02C0(r5), 0, 0
_exit_save:
    blr
}
#pragma pop

/* OSSaveFPUContext - public wrapper, just calls __OSSaveFPUContext */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSSaveFPUContext(register OSContext* context) {
    nofralloc
    /* r3 = context, map to r5 for __OSSaveFPUContext(unused1, unused2, context) */
    addi    r5, r3, 0
    b       __OSSaveFPUContext
}
#pragma pop

/* OSSetCurrentContext */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSSetCurrentContext(register OSContext* context) {
    nofralloc
    lis     r4, 0x8000
    stw     r3, 0x00D4(r4)       /* OS_CURRENTCONTEXT = context */

    clrlwi  r5, r3, 2
    stw     r5, 0x00C0(r4)       /* OS_CURRENTCONTEXT_PADDR */

    lwz     r5, 0x00D8(r4)       /* OS_FPUCONTEXT */
    cmpw    r5, r3
    bne     _not_fpu_owner

    /* context is the FPU owner: enable FPU in SRR1 and MSR */
    lwz     r6, 0x019C(r3)
    ori     r6, r6, 0x2000
    stw     r6, 0x019C(r3)
    mfmsr   r6
    ori     r6, r6, 0x0002
    mtmsr   r6
    blr

_not_fpu_owner:
    /* context is NOT the FPU owner: clear FPU bit in SRR1 */
    lwz     r6, 0x019C(r3)
    rlwinm  r6, r6, 0, 19, 17    /* clear bit 13 (FP) */
    stw     r6, 0x019C(r3)
    mfmsr   r6
    rlwinm  r6, r6, 0, 19, 17
    ori     r6, r6, 0x0002        /* set RI */
    mtmsr   r6
    isync
    blr
}
#pragma pop

/* OSLoadContext - restores full context and does rfi */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSLoadContext(register OSContext* context) {
    nofralloc
    /* Check if SRR0 points inside OSDisableInterrupts */
    lis     r4, OSDisableInterrupts@ha
    lwz     r6, 0x0198(r3)            /* srr0 */
    addi    r5, r4, OSDisableInterrupts@l
    cmplw   r6, r5
    ble     _past_check
    lis     r4, (OSDisableInterrupts+0xC)@ha
    addi    r0, r4, (OSDisableInterrupts+0xC)@l
    cmplw   r6, r0
    bge     _past_check
    stw     r5, 0x0198(r3)            /* fix to start of function */

_past_check:
    lwz     r0, 0x0000(r3)
    lwz     r1, 0x0004(r3)
    lwz     r2, 0x0008(r3)

    lhz     r4, 0x01A2(r3)
    rlwinm. r5, r4, 0, 30, 30        /* check STATE_EXC bit */
    beq     _no_exc

    /* exception state: clear EXC bit, load r5-r31 */
    rlwinm  r4, r4, 0, 31, 29
    sth     r4, 0x01A2(r3)
    lmw     r5, 0x0014(r3)
    b       _restore_spr

_no_exc:
    /* normal state: only load r13-r31 */
    lmw     r13, 0x0034(r3)

_restore_spr:
    lwz     r4, 0x01A8(r3)
    mtspr   GQR1, r4
    lwz     r4, 0x01AC(r3)
    mtspr   GQR2, r4
    lwz     r4, 0x01B0(r3)
    mtspr   GQR3, r4
    lwz     r4, 0x01B4(r3)
    mtspr   GQR4, r4
    lwz     r4, 0x01B8(r3)
    mtspr   GQR5, r4
    lwz     r4, 0x01BC(r3)
    mtspr   GQR6, r4
    lwz     r4, 0x01C0(r3)
    mtspr   GQR7, r4

    lwz     r4, 0x0080(r3)
    mtcrf   0xFF, r4
    lwz     r4, 0x0084(r3)
    mtlr    r4
    lwz     r4, 0x0088(r3)
    mtctr   r4
    lwz     r4, 0x008C(r3)
    mtxer   r4

    /* Disable FPU and RI in MSR before setting SRR0/SRR1 */
    mfmsr   r4
    rlwinm  r4, r4, 0, 17, 15        /* clear EE */
    rlwinm  r4, r4, 0, 31, 29        /* clear RI */
    mtmsr   r4

    lwz     r4, 0x0198(r3)
    mtsrr0  r4
    lwz     r4, 0x019C(r3)
    mtsrr1  r4

    lwz     r4, 0x0010(r3)
    lwz     r3, 0x000C(r3)
    rfi
}
#pragma pop

/* OSClearContext */
#pragma push
#pragma peephole off
void OSClearContext(OSContext* context) {
    context->mode  = 0;
    context->state = 0;

    if (context == OS_FPUCONTEXT) {
        OS_FPUCONTEXT = NULL;
    }
}
#pragma pop

/* OSDumpContext */
void OSDumpContext(OSContext* context) {
    u32 i;
    u32* p;

    OSReport("------------------------- Context 0x%08x -------------------------\n", context);

    for (i = 0; i < 16; ++i) {
        OSReport("r%-2d  = 0x%08x (%14d)  r%-2d  = 0x%08x (%14d)\n",
                 i, context->gpr[i], context->gpr[i],
                 i + 16, context->gpr[i + 16], context->gpr[i + 16]);
    }

    OSReport("LR   = 0x%08x                   CR   = 0x%08x\n",
             context->lr, context->cr);
    OSReport("SRR0 = 0x%08x                   SRR1 = 0x%08x\n",
             context->srr0, context->srr1);

    OSReport("\nGQRs----------\n");
    for (i = 0; i < 4; ++i) {
        OSReport("gqr%d = 0x%08x \tgqr%d = 0x%08x\n",
                 i, context->gqr[i], i + 4, context->gqr[i + 4]);
    }

    if (context->state & OS_CONTEXT_STATE_FPSAVED) {
        OSContext* currentContext;
        OSContext fpuContext;
        BOOL enabled;

        enabled = OSDisableInterrupts();
        currentContext = OSGetCurrentContext();
        OSClearContext(&fpuContext);
        OSSetCurrentContext(&fpuContext);

        OSReport("\n\nFPRs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("fr%d \t= %d \t fr%d \t= %d\n",
                     i, (u32)context->fpr[i], i + 1, (u32)context->fpr[i + 1]);
        }

        OSReport("\n\nPSFs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("ps%d \t= 0x%x \t ps%d \t= 0x%x\n",
                     i, (u32)context->psf[i], i + 1, (u32)context->psf[i + 1]);
        }

        OSClearContext(&fpuContext);
        OSSetCurrentContext(currentContext);
        OSRestoreInterrupts(enabled);
    }

    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = (u32*)context->gpr[1]; p && (u32)p != 0xFFFFFFFF && i++ < 16; p = (u32*)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }
}

/* OSSwitchFPUContext - FPU exception handler */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSSwitchFPUContext(register u8 exception, register OSContext* context) {
    nofralloc
    /* Enable FPU in MSR */
    mfmsr   r5
    ori     r5, r5, 0x2000
    mtmsr   r5
    isync

    /* Enable FPU in context's SRR1 */
    lwz     r5, 0x019C(r4)
    ori     r5, r5, 0x2000
    mtsrr1  r5

    /* Save/load FPU context if owner changed */
    lis     r3, 0x8000
    lwz     r5, 0x00D8(r3)
    stw     r4, 0x00D8(r3)
    cmpw    r5, r4
    beq     _skip_switch
    cmpwi   r5, 0
    beq     _load_only
    bl      __OSSaveFPUContext
_load_only:
    bl      __OSLoadFPUContext
_skip_switch:
    /* Restore CR, LR, SRR0, CTR, XER from context */
    lwz     r3, 0x0080(r4)
    mtcrf   0xFF, r3
    lwz     r3, 0x0084(r4)
    mtlr    r3
    lwz     r3, 0x0198(r4)
    mtsrr0  r3
    lwz     r3, 0x0088(r4)
    mtctr   r3
    lwz     r3, 0x008C(r4)
    mtxer   r3

    /* Clear FPSAVED bit */
    lhz     r3, 0x01A2(r4)
    rlwinm  r3, r3, 0, 31, 29
    sth     r3, 0x01A2(r4)

    /* Restore r3, r4, r5 and return */
    lwz     r5, 0x0014(r4)
    lwz     r3, 0x000C(r4)
    lwz     r4, 0x0010(r4)
    rfi
}
#pragma pop

/* __OSContextInit */
void __OSContextInit(void) {
    __OSSetExceptionHandler(OS_EXCEPTION_FLOATING_POINT,
                            (__OSExceptionHandler)OSSwitchFPUContext);
    OS_FPUCONTEXT = NULL;
    DBPrintf("FPU-unavailable handler installed\n");
}

/* OSGetCurrentContext */
OSContext* OSGetCurrentContext(void) {
    return OS_CURRENTCONTEXT;
}

/* OSSaveContext */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 OSSaveContext(register OSContext* context) {
    nofralloc
    stmw    r13, 0x0034(r3)

    mfspr   r0, GQR1
    stw     r0, 0x01A8(r3)
    mfspr   r0, GQR2
    stw     r0, 0x01AC(r3)
    mfspr   r0, GQR3
    stw     r0, 0x01B0(r3)
    mfspr   r0, GQR4
    stw     r0, 0x01B4(r3)
    mfspr   r0, GQR5
    stw     r0, 0x01B8(r3)
    mfspr   r0, GQR6
    stw     r0, 0x01BC(r3)
    mfspr   r0, GQR7
    stw     r0, 0x01C0(r3)

    mfcr    r0
    stw     r0, 0x0080(r3)
    mflr    r0
    stw     r0, 0x0084(r3)
    stw     r0, 0x0198(r3)
    mfmsr   r0
    stw     r0, 0x019C(r3)
    mfctr   r0
    stw     r0, 0x0088(r3)
    mfxer   r0
    stw     r0, 0x008C(r3)

    stw     r1, 0x0004(r3)
    stw     r2, 0x0008(r3)

    li      r0, 1
    stw     r0, 0x000C(r3)
    li      r3, 0
    blr
}
#pragma pop

/* OSGetStackPointer */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 OSGetStackPointer(void) {
    nofralloc
    mr      r3, r1
    blr
}
#pragma pop

/* OSSwitchFiber */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm int OSSwitchFiber(register u32 pc, register u32 newsp) {
    nofralloc
    mflr    r0
    mr      r5, r1
    stwu    r5, -8(r4)
    mr      r1, r4
    stw     r0, 4(r5)
    mtlr    r3
    blrl
    lwz     r5, 0(r1)
    lwz     r0, 4(r5)
    mtlr    r0
    mr      r1, r5
    blr
}
#pragma pop

/* OSInitContext */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSInitContext(register OSContext* context, register u32 pc, register u32 newsp) {
    nofralloc
    stw     r4, 0x0198(r3)
    stw     r5, 0x0004(r3)
    li      r11, 0
    ori     r11, r11, 0x9032
    stw     r11, 0x019C(r3)
    li      r0, 0
    stw     r0, 0x0080(r3)
    stw     r0, 0x008C(r3)
    stw     r2, 0x0008(r3)
    stw     r13, 0x0034(r3)
    stw     r0, 0x000C(r3)
    stw     r0, 0x0010(r3)
    stw     r0, 0x0014(r3)
    stw     r0, 0x0018(r3)
    stw     r0, 0x001C(r3)
    stw     r0, 0x0020(r3)
    stw     r0, 0x0024(r3)
    stw     r0, 0x0028(r3)
    stw     r0, 0x002C(r3)
    stw     r0, 0x0030(r3)
    stw     r0, 0x0038(r3)
    stw     r0, 0x003C(r3)
    stw     r0, 0x0040(r3)
    stw     r0, 0x0044(r3)
    stw     r0, 0x0048(r3)
    stw     r0, 0x004C(r3)
    stw     r0, 0x0050(r3)
    stw     r0, 0x0054(r3)
    stw     r0, 0x0058(r3)
    stw     r0, 0x005C(r3)
    stw     r0, 0x0060(r3)
    stw     r0, 0x0064(r3)
    stw     r0, 0x0068(r3)
    stw     r0, 0x006C(r3)
    stw     r0, 0x0070(r3)
    stw     r0, 0x0074(r3)
    stw     r0, 0x0078(r3)
    stw     r0, 0x007C(r3)
    stw     r0, 0x01A4(r3)
    stw     r0, 0x01A8(r3)
    stw     r0, 0x01AC(r3)
    stw     r0, 0x01B0(r3)
    stw     r0, 0x01B4(r3)
    stw     r0, 0x01B8(r3)
    stw     r0, 0x01BC(r3)
    stw     r0, 0x01C0(r3)
    b       OSClearContext
}
#pragma pop

