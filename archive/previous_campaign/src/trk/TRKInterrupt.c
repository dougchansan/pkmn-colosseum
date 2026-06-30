#include "dolphin/types.h"

/*
 * TRKInterrupt.c - TRK interrupt and exception handling.
 *
 * These functions handle hardware interrupts and exceptions during
 * debugging. They save/restore CPU state and coordinate with the
 * TRK event system. All functions here are pure assembly because
 * they operate at exception level and manipulate SPRs directly.
 */

/* External references used by interrupt handlers */
extern u8 gTRKState[];     /* TRK global state structure */
extern u8 gTRKCPUState[];  /* saved CPU context */
extern u8 gTRKSaveState[]; /* secondary save state */
extern u16 TRK_saved_exceptionID;
extern u8 gTRKExceptionStatus[]; /* at 80313824, +0x0C = inException, +0x0D = exceptionOccurred */

extern void TRKSaveExtended1Block(void);
extern void TRKRestoreExtended1Block(void);
extern void TRKPostInterruptEvent(void);
extern void TRKUARTInterruptHandler(void);

/* Forward declarations for asm functions defined below */
asm void TRKExceptionHandler(void);
asm void TRKInterruptHandlerEnableInterrupts(void);

/*
 * TRKInterruptHandler - Main interrupt vector entry point.
 *
 * Called from exception vectors. Saves minimal state using SPRGs,
 * determines the exception type, and either:
 *   - Handles external interrupts (0x500) by calling TRKUARTInterruptHandler
 *   - Saves full CPU state and transitions to the debugger
 *   - Re-enters a nested exception via TRKExceptionHandler
 *
 * Entry: r3 = exception ID, r4 = saved SRR1 (via SPRG)
 *        SRR0 saved to r2 (via SPRG1)
 */
asm void TRKInterruptHandler(void) {
    nofralloc

    /* Save SRR0 and original r4 via SPRGs */
    mtsrr0  r2
    mtsrr1  r4
    mfsprg  r4, 3
    mfcr    r2
    mtsprg  3, r2

    /* Disable EE and RI in MSR for safe exception handling */
    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l
    lwz     r2, 0x8C(r2)       /* load saved MSR from state */
    ori     r2, r2, 0x8002     /* set EE and RI bits */
    xori    r2, r2, 0x8002     /* then clear them */
    sync
    mtmsr   r2
    sync

    /* Save exception ID */
    lis     r2, TRK_saved_exceptionID@h
    ori     r2, r2, TRK_saved_exceptionID@l
    sth     r3, 0(r2)

    /* Check if this is an external interrupt (0x500) */
    cmpwi   r3, 0x500
    bne     @not_ext_interrupt

    /* Handle external interrupt: call UART handler */
    lis     r2, gTRKCPUState@h
    ori     r2, r2, gTRKCPUState@l
    mflr    r3
    stw     r3, 0x42C(r2)      /* save LR */
    bl      TRKUARTInterruptHandler
    lis     r2, gTRKCPUState@h
    ori     r2, r2, gTRKCPUState@l
    lwz     r3, 0x42C(r2)      /* restore LR */
    mtlr    r3

    /* Check if input is pending */
    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l
    lwz     r2, 0xA0(r2)       /* gTRKState.inputPendingPtr */
    lbz     r2, 0(r2)
    cmpwi   r2, 0
    beq     @return_from_interrupt

    /* Check if already in exception handler */
    lis     r2, gTRKExceptionStatus@h
    ori     r2, r2, gTRKExceptionStatus@l
    lbz     r2, 0x0C(r2)       /* inException flag */
    cmpwi   r2, 1
    beq     @return_from_interrupt

    /* Set flag to notify main loop of pending input */
    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l
    li      r3, 1
    stb     r3, 0x9C(r2)       /* inputNotify flag */
    b       @not_ext_interrupt

@return_from_interrupt:
    /* Restore state from save area and return via rfi */
    lis     r2, gTRKSaveState@h
    ori     r2, r2, gTRKSaveState@l
    lwz     r3, 0x88(r2)       /* saved CR */
    mtcrf   255, r3
    lwz     r3, 0x0C(r2)       /* saved r3 */
    lwz     r2, 0x08(r2)       /* saved r2 */
    rfi

@not_ext_interrupt:
    /* Load saved exception ID */
    lis     r2, TRK_saved_exceptionID@h
    ori     r2, r2, TRK_saved_exceptionID@l
    lhz     r3, 0(r2)

    /* Check if already in exception handler */
    lis     r2, gTRKExceptionStatus@h
    ori     r2, r2, gTRKExceptionStatus@l
    lbz     r2, 0x0C(r2)
    cmpwi   r2, 0
    bne     TRKExceptionHandler

    /* Save full CPU state */
    lis     r2, gTRKCPUState@h
    ori     r2, r2, gTRKCPUState@l
    stw     r0, 0x00(r2)       /* GPR0 */
    stw     r1, 0x04(r2)       /* GPR1 */
    mfsprg  r0, 1              /* original r2 from SPRG1 */
    stw     r0, 0x08(r2)       /* GPR2 */
    sth     r3, 0x2F8(r2)      /* exception ID */
    sth     r3, 0x2FA(r2)      /* exception ID copy */
    mfsprg  r0, 2              /* original r3 from SPRG2 */
    stw     r0, 0x0C(r2)       /* GPR3 */
    stmw    r4, 0x10(r2)       /* GPR4-GPR31 */

    /* Save special registers */
    mfsrr0  r27
    mflr    r28
    mfsprg  r29, 3             /* saved CR */
    mfctr   r30
    mfxer   r31
    stmw    r27, 0x80(r2)      /* SRR0, LR, CR, CTR, XER */

    /* Save extended state (SPRs, BATs, etc.) */
    bl      TRKSaveExtended1Block

    /* Mark as in exception handler */
    lis     r2, gTRKExceptionStatus@h
    ori     r2, r2, gTRKExceptionStatus@l
    li      r3, 1
    stb     r3, 0x0C(r2)

    /* Restore MSR to normal debugger mode */
    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l
    lwz     r0, 0x8C(r2)
    sync
    mtmsr   r0
    sync

    /* Restore special registers for debugger use */
    lwz     r0, 0x80(r2)
    mtlr    r0
    lwz     r0, 0x84(r2)
    mtctr   r0
    lwz     r0, 0x88(r2)
    mtxer   r0
    lwz     r0, 0x94(r2)
    mtdsisr r0
    lwz     r0, 0x90(r2)
    mtdar   r0

    /* Restore GPRs and post interrupt event */
    lmw     r3, 0x0C(r2)
    lwz     r0, 0x00(r2)
    lwz     r1, 0x04(r2)
    lwz     r2, 0x08(r2)
    b       TRKPostInterruptEvent
}

/*
 * TRKExceptionHandler - Handle nested exceptions during debugging.
 *
 * Called when an exception occurs while already in exception context.
 * Records the exception info, advances SRR0 past the faulting instruction
 * for certain exception types, and returns via rfi.
 */
asm void TRKExceptionHandler(void) {
    nofralloc

    lis     r2, gTRKExceptionStatus@h
    ori     r2, r2, gTRKExceptionStatus@l
    sth     r3, 0x08(r2)       /* save exception ID */

    mfsrr0  r3
    stw     r3, 0x00(r2)       /* save faulting PC */

    lhz     r3, 0x08(r2)       /* reload exception ID */

    /* For certain exceptions, skip the faulting instruction */
    cmpwi   r3, 0x200
    beq     @skip_instruction
    cmpwi   r3, 0x300
    beq     @skip_instruction
    cmpwi   r3, 0x400
    beq     @skip_instruction
    cmpwi   r3, 0x600
    beq     @skip_instruction
    cmpwi   r3, 0x700
    beq     @skip_instruction
    cmpwi   r3, 0x800
    beq     @skip_instruction
    cmpwi   r3, 0x1000
    beq     @skip_instruction
    cmpwi   r3, 0x1100
    beq     @skip_instruction
    cmpwi   r3, 0x1200
    beq     @skip_instruction
    cmpwi   r3, 0x1300
    beq     @skip_instruction
    b       @no_skip

@skip_instruction:
    mfsrr0  r3
    addi    r3, r3, 4
    mtsrr0  r3

@no_skip:
    /* Mark that an exception occurred */
    lis     r2, gTRKExceptionStatus@h
    ori     r2, r2, gTRKExceptionStatus@l
    li      r3, 1
    stb     r3, 0x0D(r2)       /* exceptionOccurred = TRUE */

    /* Restore CR and r2/r3 from SPRGs, return */
    mfsprg  r3, 3              /* saved CR */
    mtcrf   255, r3
    mfsprg  r2, 1              /* original r2 */
    mfsprg  r3, 2              /* original r3 */
    rfi
}

/*
 * TRKSwapAndGo - Save debugger state and resume target execution.
 *
 * Saves all debugger registers into gTRKState, disables interrupts,
 * checks if input is pending, and either:
 *   - Enables interrupts and posts an event (if input pending)
 *   - Restores the full target CPU state and resumes via rfi
 */
asm void TRKSwapAndGo(void) {
    nofralloc

    /* Save all GPRs to gTRKState */
    lis     r3, gTRKState@h
    ori     r3, r3, gTRKState@l
    stmw    r0, 0x00(r3)

    /* Save special registers */
    mfmsr   r0
    stw     r0, 0x8C(r3)       /* MSR */
    mflr    r0
    stw     r0, 0x80(r3)       /* LR */
    mfctr   r0
    stw     r0, 0x84(r3)       /* CTR */
    mfxer   r0
    stw     r0, 0x88(r3)       /* XER */
    mfdsisr r0
    stw     r0, 0x94(r3)       /* DSISR */
    mfdar   r0
    stw     r0, 0x90(r3)       /* DAR */

    /* Disable EE and RI in MSR */
    li      r1, -0x7FFE        /* 0xFFFF8002 */
    nor     r1, r1, r1         /* complement to 0x00007FFD */
    mfmsr   r3
    and     r3, r3, r1         /* clear EE and RI */
    mtmsr   r3

    /* Check if input is pending */
    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l
    lwz     r2, 0xA0(r2)       /* inputPendingPtr */
    lbz     r2, 0(r2)
    cmpwi   r2, 0
    beq     @do_restore

    /* Input pending: set notify flag and enable interrupts */
    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l
    li      r3, 1
    stb     r3, 0x9C(r2)
    b       TRKInterruptHandlerEnableInterrupts

@do_restore:
    /* Clear exception handler flag */
    lis     r2, gTRKExceptionStatus@h
    ori     r2, r2, gTRKExceptionStatus@l
    li      r3, 0
    stb     r3, 0x0C(r2)

    /* Restore extended state */
    bl      TRKRestoreExtended1Block

    /* Restore SPRs and GPRs from CPU state */
    lis     r2, gTRKCPUState@h
    ori     r2, r2, gTRKCPUState@l
    lmw     r27, 0x80(r2)
    mtsrr0  r27
    mtlr    r28
    mtcrf   255, r29
    mtctr   r30
    mtxer   r31

    lmw     r3, 0x0C(r2)
    lwz     r0, 0x00(r2)
    lwz     r1, 0x04(r2)
    lwz     r2, 0x08(r2)
    rfi
}

/*
 * TRKInterruptHandlerEnableInterrupts - Re-enable interrupts and
 * post an interrupt event to the debugger.
 *
 * Restores the saved MSR and all registers from gTRKState, then
 * branches to TRKPostInterruptEvent.
 */
asm void TRKInterruptHandlerEnableInterrupts(void) {
    nofralloc

    lis     r2, gTRKState@h
    ori     r2, r2, gTRKState@l

    /* Restore MSR */
    lwz     r0, 0x8C(r2)
    sync
    mtmsr   r0
    sync

    /* Restore special registers */
    lwz     r0, 0x80(r2)
    mtlr    r0
    lwz     r0, 0x84(r2)
    mtctr   r0
    lwz     r0, 0x88(r2)
    mtxer   r0
    lwz     r0, 0x94(r2)
    mtdsisr r0
    lwz     r0, 0x90(r2)
    mtdar   r0

    /* Restore GPRs */
    lmw     r3, 0x0C(r2)
    lwz     r0, 0x00(r2)
    lwz     r1, 0x04(r2)
    lwz     r2, 0x08(r2)

    b       TRKPostInterruptEvent
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800C11F4 - 0x800C11F4 | size: 0x24 */
void fn_800C11F4(void) {
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f31 = 0.0f;

    /* mffs f31 */;
    *(f64*)((u8*)r3 + 0x0) = f31;
    return;
}

/* fn_800C1218 - 0x800C1218 | size: 0x24 */
void fn_800C1218(void) {
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f31 = 0.0f;

    f31 = *(f64*)((u8*)r3 + 0x0);
    /* mtfsf 255, f31 */;
    return;
}

/* fn_800C123C - 0x800C123C | size: 0xC4
 * TRKDispatchMessage - Dispatch a TRK command.
 * Saves/restores exception status, calls the appropriate handler
 * based on whether the target is running, and checks for exceptions
 * that occurred during handling.
 */
u32 fn_800C123C(u32 cmd, u32 arg1, u32* resultOut) {
    extern u8 gTRKExceptionStatus_80313824[];
    extern void fn_800C2EAC(void);
    extern void fn_800C3098(void);
    u8* status = gTRKExceptionStatus_80313824;
    u32 result;
    u32 savedPC;
    u32 savedField4;
    u32 savedField8;
    u32 savedInExc;

    result = 0;

    /* Save exception status fields */
    savedInExc = *(u32*)(status + 0xC);
    savedPC    = *(u32*)(status + 0x0);
    savedField4 = *(u32*)(status + 0x4);
    savedField8 = *(u32*)(status + 0x8);

    /* Clear exceptionOccurred flag */
    *(u8*)(status + 0xD) = 0;

    /* Dispatch based on whether already in exception */
    if (savedInExc != 0) {
        fn_800C3098();
    } else {
        fn_800C2EAC();
    }

    /* Check if an exception occurred during dispatch */
    if (*(u8*)(status + 0xD) != 0) {
        result = 0x702;
        *resultOut = 0;
    }

    /* Restore exception status */
    *(u32*)(status + 0x0) = savedInExc;
    *(u32*)(status + 0x4) = arg1;
    *(u32*)(status + 0x8) = cmd;
    *(u32*)(status + 0xC) = *(u8*)(status + 0xD);

    return result;
}

