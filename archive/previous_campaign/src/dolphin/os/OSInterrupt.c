#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"

extern void* memset(void* dest, int val, u32 n);

/*
 * OSInterrupt.c - Interrupt management.
 *
 * Manages the interrupt handler table, interrupt masking, and provides
 * the external interrupt exception handler.
 *
 * Matches: 0x8009DF3C - 0x8009E7A4
 */

static __OSInterruptHandler* InterruptHandlerTable;

/* SDA symbol aliases used by stub functions */
extern u32 InterruptHandlerTable_8047A710;
extern u16 __OSLastInterrupt;
extern u32 __OSLastInterruptTime;
extern u32 __OSLastInterruptSrr0;

/* Hardware registers */
#define PI_INTMR    (*(volatile u32*)0xCC003004)

extern void __OSDispatchInterrupt(u8 exception, OSContext* context);

/* __OSSetInterruptMask - translates OS interrupt bits to hardware register writes.
 * This is fn_8009E02C in the disassembly, a large function that maps OS-level
 * interrupt mask bits to the appropriate PI/DSP/AI/SI/EXI hardware registers.
 * Takes the changed mask bits and the combined global mask, returns remaining
 * unprocessed bits.
 */
extern u32 __OSSetInterruptMask(u32 mask, u32 globalMask);
extern void fn_8009E414(void);

/* Forward declaration */
static void ExternalInterruptHandler(u8 exception, OSContext* context);

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm BOOL OSDisableInterrupts(void) {
    nofralloc
    mfmsr   r3
    rlwinm  r4, r3, 0, 17, 15  /* clear MSR[EE] */
    mtmsr   r4
    extrwi  r3, r3, 1, 16      /* extract MSR[EE] bit */
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm BOOL OSEnableInterrupts(void) {
    nofralloc
    mfmsr   r3
    ori     r4, r3, 0x8000      /* set MSR[EE] */
    mtmsr   r4
    extrwi  r3, r3, 1, 16       /* extract old MSR[EE] bit */
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm BOOL OSRestoreInterrupts(register BOOL level) {
    nofralloc
    cmpwi   r3, 0
    mfmsr   r4
    beq     _disable
    ori     r5, r4, 0x8000       /* set MSR[EE] */
    b       _set
_disable:
    rlwinm  r5, r4, 0, 17, 15   /* clear MSR[EE] */
_set:
    mtmsr   r5
    extrwi  r3, r4, 1, 16       /* return old MSR[EE] bit */
    blr
}
#pragma pop

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler) {
    __OSInterruptHandler* table = (__OSInterruptHandler*)InterruptHandlerTable_8047A710;
    __OSInterruptHandler old;

    old = table[interrupt];
    table[interrupt] = handler;
    return old;
}

__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt) {
    return ((__OSInterruptHandler*)InterruptHandlerTable_8047A710)[interrupt];
}

void __OSInterruptInit(void) {
    InterruptHandlerTable = (__OSInterruptHandler*)0x80003040;
    memset(InterruptHandlerTable, 0, 32 * sizeof(__OSInterruptHandler));

    *(volatile u32*)0x800000C4 = 0;
    *(volatile u32*)0x800000C8 = 0;
    PI_INTMR = 0xF0;

    __OSMaskInterrupts(0xFFFFFFE0);
    __OSSetExceptionHandler(OS_EXCEPTION_EXTERNAL_INTERRUPT,
                            (__OSExceptionHandler)ExternalInterruptHandler);
}

u32 __OSMaskInterrupts(u32 global) {
#pragma peephole off
    extern u32 SetInterruptMask(u32 mask, u32 globalMask);
    BOOL enabled;
    u32 prev;
    u32 local;
    u32 mask;

    enabled = OSDisableInterrupts();

    prev  = *(volatile u32*)0x800000C4;
    local = *(volatile u32*)0x800000C8;
    mask = ~(prev | local) & global;
    global |= prev;
    *(volatile u32*)0x800000C4 = global;

    while (mask) {
        mask = SetInterruptMask(mask, global | local);
    }

    OSRestoreInterrupts(enabled);
    return prev;
}

u32 __OSUnmaskInterrupts(u32 global) {
#pragma peephole off
    extern u32 SetInterruptMask(u32 mask, u32 globalMask);
    BOOL enabled;
    u32 prev;
    u32 local;
    u32 mask;

    enabled = OSDisableInterrupts();

    prev  = *(volatile u32*)0x800000C4;
    local = *(volatile u32*)0x800000C8;
    mask = (prev | local) & global;
    global = prev & ~global;
    *(volatile u32*)0x800000C4 = global;

    while (mask) {
        mask = SetInterruptMask(mask, global | local);
    }

    OSRestoreInterrupts(enabled);
    return prev;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm static void ExternalInterruptHandler(register u8 exception, register OSContext* context) {
    nofralloc
    stw     r0,   0x0000(r4)
    stw     r1,   0x0004(r4)
    stw     r2,   0x0008(r4)
    stmw    r6,   0x0018(r4)
    mfspr   r0, GQR1
    stw     r0, 0x01A8(r4)
    mfspr   r0, GQR2
    stw     r0, 0x01AC(r4)
    mfspr   r0, GQR3
    stw     r0, 0x01B0(r4)
    mfspr   r0, GQR4
    stw     r0, 0x01B4(r4)
    mfspr   r0, GQR5
    stw     r0, 0x01B8(r4)
    mfspr   r0, GQR6
    stw     r0, 0x01BC(r4)
    mfspr   r0, GQR7
    stw     r0, 0x01C0(r4)
    stwu    r1, -0x0008(r1)
    b       fn_8009E414
}
#pragma pop

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 1 function(s)
 * =================================================================== */

/* fn_8009E414 - 0x8009E414 | size: 0x344 */
void fn_8009E414(void) {
    extern u8 lbl_803117E8[];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r12 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r4;
    r3 = 0xCC000000;
    r31 = *(u32*)((u8*)r3 + 0x3000);
    r31 = r31 & 0xFFFEFFFF;
    if (r31 == 0 || (r31 & *(u32*)(0xCC003000 + 0x4)) == 0) {
        r3 = r30;
        OSLoadContext((OSContext*)r3);
    }
    tmp = r31 & 0x00000080;
    tmp = 0x0;
    if (tmp != 0) {
        r3 = 0xCC000000;
        r3 = r3 + 0x4000;
        r4 = *(u16*)((u8*)r3 + 0x1E);
        r3 = r4 & 0x1;
        if (r3 != 0) {
            tmp = tmp | (0x8000 << 16);
        }
        r3 = r4 & 0x00000002;
        if (r3 != 0) {
            tmp = tmp | (0x4000 << 16);
        }
        r3 = r4 & 0x00000004;
        if (r3 != 0) {
            tmp = tmp | (0x2000 << 16);
        }
        r3 = r4 & 0x00000008;
        if (r3 != 0) {
            tmp = tmp | (0x1000 << 16);
        }
        r3 = r4 & 0x00000010;
        if (r3 != 0) {
            tmp = tmp | (0x800 << 16);
    }
    }
    r3 = r31 & 0x00000040;
    if (r3 != 0) {
        r3 = 0xCC000000;
        r3 = r3 + 0x5000;
        r4 = *(u16*)((u8*)r3 + 0xA);
        r3 = r4 & 0x00000008;
        if (r3 != 0) {
            tmp = tmp | (0x400 << 16);
        }
        r3 = r4 & 0x00000020;
        if (r3 != 0) {
            tmp = tmp | (0x200 << 16);
        }
        r3 = r4 & 0x00000080;
        if (r3 != 0) {
            tmp = tmp | (0x100 << 16);
    }
    }
    r3 = r31 & 0x00000020;
    if (r3 != 0) {
        r3 = 0xCC000000;
        r3 = *(u32*)((u8*)r3 + 0x6C00);
        r3 = r3 & 0x00000008;
        if (r3 != 0) {
            tmp = tmp | (0x80 << 16);
    }
    }
    r3 = r31 & 0x00000010;
    if (r3 != 0) {
        r3 = 0xCC000000;
        r4 = *(u32*)((u8*)r3 + 0x6800);
        r3 = r4 & 0x00000002;
        if (r3 != 0) {
            tmp = tmp | (0x40 << 16);
        }
        r3 = r4 & 0x00000008;
        if (r3 != 0) {
            tmp = tmp | (0x20 << 16);
        }
        r3 = r4 & 0x00000800;
        if (r3 != 0) {
            tmp = tmp | (0x10 << 16);
        }
        r3 = 0xCC000000;
        r3 = r3 + 0x6800;
        r4 = *(u32*)((u8*)r3 + 0x14);
        r3 = r4 & 0x00000002;
        if (r3 != 0) {
            tmp = tmp | (0x8 << 16);
        }
        r3 = r4 & 0x00000008;
        if (r3 != 0) {
            tmp = tmp | (0x4 << 16);
        }
        r3 = r4 & 0x00000800;
        if (r3 != 0) {
            tmp = tmp | (0x2 << 16);
        }
        r3 = 0xCC000000;
        r3 = r3 + 0x6800;
        r4 = *(u32*)((u8*)r3 + 0x28);
        r3 = r4 & 0x00000002;
        if (r3 != 0) {
            tmp = tmp | (0x1 << 16);
        }
        r3 = r4 & 0x00000008;
        if (r3 != 0) {
            tmp = tmp | 0x8000;
    }
    }
    r3 = r31 & 0x00002000;
    if (r3 != 0) {
        tmp = tmp | 0x20;
    }
    r3 = r31 & 0x00001000;
    if (r3 != 0) {
        tmp = tmp | 0x40;
    }
    r3 = r31 & 0x00000400;
    if (r3 != 0) {
        tmp = tmp | 0x1000;
    }
    r3 = r31 & 0x00000200;
    if (r3 != 0) {
        tmp = tmp | 0x2000;
    }
    r3 = r31 & 0x00000100;
    if (r3 != 0) {
        tmp = tmp | 0x80;
    }
    r3 = r31 & 0x00000008;
    if (r3 != 0) {
        tmp = tmp | 0x800;
    }
    r3 = r31 & 0x00000004;
    if (r3 != 0) {
        tmp = tmp | 0x400;
    }
    r3 = r31 & 0x00000002;
    if (r3 != 0) {
        tmp = tmp | 0x200;
    }
    r3 = r31 & 0x00000800;
    if (r3 != 0) {
        tmp = tmp | 0x4000;
    }
    r3 = r31 & 0x1;
    if (r3 != 0) {
        tmp = tmp | 0x100;
    }
    r3 = 0x80000000;
    r4 = *(u32*)((u8*)r3 + 0xC4);
    r3 = *(u32*)((u8*)r3 + 0xC8);
    r3 = r4 | r3;
    r4 = tmp & ~r3;
    if (r4 != 0) {
        r3 = (u32)lbl_803117E8;
        tmp = (u32)lbl_803117E8;
        r3 = tmp;

        /* Scan interrupt mask table to find pending interrupt */
        while (1) {
            tmp = *(u32*)((u8*)r3 + 0x0);
            tmp = r4 & tmp;
            if (tmp != 0) {
                tmp = __cntlzw(tmp);
                r29 = (s16)tmp;
                break;
            }
            r3 = r3 + 0x4;
        }

        r3 = *(u32*)InterruptHandlerTable_8047A710;
        tmp = r29 << 2;
        r31 = *(u32*)(r3 + tmp);
        if (r31 != 0) {
            if ((s32)r29 > 4) {
                *(u16*)__OSLastInterrupt = r29;
                OSGetTime();
                *((u32*)&__OSLastInterruptTime + 1) = r4;
                *(u32*)__OSLastInterruptTime = r3;
                tmp = *(u32*)((u8*)r30 + 0x198);
                *(u32*)__OSLastInterruptSrr0 = tmp;
            }
            OSDisableScheduler();
            r3 = r29;
            r4 = r30;
            r12 = r31;
            /* blrl  */;
            OSEnableScheduler();
            __OSReschedule();
            r3 = r30;
            OSLoadContext((OSContext*)r3);
        }
    }
    r3 = r30;
    OSLoadContext((OSContext*)r3);
    return;
}
