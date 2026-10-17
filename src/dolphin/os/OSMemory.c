#include "dolphin/os/OSMemory.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSReset.h"
#include "dolphin/os/OSThread.h"

/*
 * OSMemory.c - Memory protection and BAT configuration.
 *
 * Sets up the memory protection registers, BAT registers for 24MB/48MB
 * memory configurations, and the memory protection interrupt handler.
 *
 * Matches: 0x8009F1B8 - 0x8009F77C
 */

#define TRUNC(n, a) (((u32)(n)) & ~((a)-1))
#define ROUND(n, a) (((u32)(n) + (a)-1) & ~((a)-1))

/* Memory protection hardware registers */
volatile u16 __MEMRegs[64] : (0xCC004000);

#define OS_PROTECT_CONTROL_RDWR 0x03

#define __OS_INTERRUPT_MEM_0       0
#define __OS_INTERRUPT_MEM_1       1
#define __OS_INTERRUPT_MEM_2       2
#define __OS_INTERRUPT_MEM_3       3
#define __OS_INTERRUPT_MEM_ADDRESS 4

#define OS_INTERRUPTMASK(interrupt) (0x80000000u >> (interrupt))
#define OS_INTERRUPTMASK_MEM_0       OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0)
#define OS_INTERRUPTMASK_MEM_1       OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_1)
#define OS_INTERRUPTMASK_MEM_2       OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_2)
#define OS_INTERRUPTMASK_MEM_3       OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_3)
#define OS_INTERRUPTMASK_MEM_ADDRESS OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_ADDRESS)
#define OS_INTERRUPTMASK_MEM_RESET \
    (OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 | OS_INTERRUPTMASK_MEM_3)

/* Error table for memory protection */
extern OSErrorHandler __OSErrorTable[];

#if defined(OSMEMORY_PRIVILEGED_ONLY)
/* OSMemory_privileged.c builds only Config24MB .. __OSInitMemoryProtection;
   the handler and the reset record are linked from OSMemory_exact_8009F1B8.c
   and data_80311868.c under their extracted names. */
#define ResetFunctionInfo ResetFunctionInfo_80311868
#define MEMIntrruptHandler MEMIntrruptHandler_8009F41C
extern OSResetFunctionInfo ResetFunctionInfo;
void MEMIntrruptHandler(s16 interrupt, OSContext* context);
#else
/* Reset function info for memory protection */
extern OSResetFunctionInfo ResetFunctionInfo;

static void MEMIntrruptHandler(s16 interrupt, OSContext* context);
#endif
void Config24MB(void);
void Config48MB(void);
void RealMode(void* target);

#if !defined(OSMEMORY_PRIVILEGED_ONLY)
typedef struct OSModuleInfo OSModuleInfo;
typedef struct OSModuleQueue {
    OSModuleInfo* head;
    OSModuleInfo* tail;
} OSModuleQueue;

OSModuleQueue __OSModuleInfoList : (0x800030C8);
const void* __OSStringTable : (0x800030D0);

void __OSModuleInit(void) {
    __OSModuleInfoList.head = __OSModuleInfoList.tail = 0;
    __OSStringTable = 0;
}

extern void OSInitThreadQueue();
void OSInitMessageQueue(u8* ptr, u32 val1, u32 val2) {
    OSInitThreadQueue(ptr);
    OSInitThreadQueue(ptr + 0x8);
    *(u32*)(ptr + 0x10) = val1;
    *(u32*)(ptr + 0x14) = val2;
    *(u32*)(ptr + 0x18) = 0;
    *(u32*)(ptr + 0x1C) = 0;
}

static void MEMIntrruptHandler(s16 interrupt, OSContext* context) {
    u32 cause;
    u32 addr;

    cause = __MEMRegs[0xF];
    addr = (((u32)__MEMRegs[0x12] & 0x3FF) << 16) | __MEMRegs[0x11];
    __MEMRegs[0x10] = 0;

    if (__OSErrorTable[OS_ERROR_PROTECTION] != NULL) {
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, cause, addr);
        return;
    }

    __OSUnhandledException(OS_ERROR_PROTECTION, context, cause, addr);
}

#if 0
asm void fn_8009F488(void) {
#include "src/dolphin/os/OSMemory_fn_8009F488.inc"
}
#else
void OSProtectRange(u32 chan, void* addr, u32 nBytes, u32 control) {
    BOOL enabled;
    u32 start;
    u32 end;
    u16 reg;

    if (4 <= chan) {
        return;
    }

    control &= 3;

    end = (u32)addr + nBytes;
    start = TRUNC(addr, 1u << 10);
    end = ROUND(end, 1u << 10);

    DCFlushRange((void*)start, end - start);

    enabled = OSDisableInterrupts();

    __OSMaskInterrupts(OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0 + chan));

    __MEMRegs[0 + 2 * chan] = (u16)(start >> 10);
    __MEMRegs[1 + 2 * chan] = (u16)(end >> 10);

    reg = __MEMRegs[8];
    reg &= ~(3 << 2 * chan);
    reg |= control << 2 * chan;
    __MEMRegs[8] = reg;

    if (control != 3) {
        __OSUnmaskInterrupts(OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0 + chan));
    }

    OSRestoreInterrupts(enabled);
}
#endif
#endif /* !OSMEMORY_PRIVILEGED_ONLY */

/* Config24MB - sets BAT registers for 24MB physical memory layout.
   Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm void Config24MB(void) {
    nofralloc
    li      r7, 0

    /* DBAT0: 0x80000000, 16MB */
    lis     r4, 0x0000
    addi    r4, r4, 0x0002
    lis     r3, 0x8000
    addi    r3, r3, 0x01FF
    /* Pre-load DBAT2/IBAT2 values */
    lis     r6, 0x0100
    addi    r6, r6, 0x0002
    lis     r5, 0x8100
    addi    r5, r5, 0x00FF
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3

    /* DBAT2: 0x81000000, 8MB */
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5

    /* Return via rfi to re-enable address translation */
    isync
    mfmsr   r3
    ori     r3, r3, 0x0030
    mtsrr1  r3
    mflr    r3
    mtsrr0  r3
    rfi
}

/* Config48MB - sets BAT registers for 48MB physical memory layout.
   Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm void Config48MB(void) {
    nofralloc
    li      r7, 0

    /* DBAT0: 0x80000000, 32MB */
    lis     r4, 0x0000
    addi    r4, r4, 0x0002
    lis     r3, 0x8000
    addi    r3, r3, 0x03FF
    /* Pre-load DBAT2/IBAT2 values */
    lis     r6, 0x0200
    addi    r6, r6, 0x0002
    lis     r5, 0x8200
    addi    r5, r5, 0x01FF
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3

    /* DBAT2: 0x82000000, 16MB */
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5

    /* Return via rfi */
    isync
    mfmsr   r3
    ori     r3, r3, 0x0030
    mtsrr1  r3
    mflr    r3
    mtsrr0  r3
    rfi
}

/* RealMode - enter real mode (disable address translation) then jump to target.
   Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm void RealMode(register void* target) {
    nofralloc
    clrlwi  r3, r3, 2         /* mask to physical address */
    mtsrr0  r3
    mfmsr   r3
    rlwinm  r3, r3, 0, 28, 25 /* clear IR, DR bits */
    mtsrr1  r3
    rfi
}

void __OSInitMemoryProtection(void) {
#ifndef DEBUG
    u32 padding[11];
    u32 temp;
#endif
    u32 size;
    BOOL enabled;

    size = *(volatile u32*)0x800000F0;

    enabled = OSDisableInterrupts();

    __MEMRegs[16] = 0;
    __MEMRegs[8] = 0xFF;

    __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                        OS_INTERRUPTMASK_MEM_3);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_0, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_1, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_2, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_3, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_ADDRESS, (__OSInterruptHandler)MEMIntrruptHandler);
    OSRegisterResetFunction(&ResetFunctionInfo);

    {
        u32 physMemSize = *(volatile u32*)0x800000F0;
        u32 memSizeField = *(volatile u32*)0x80000028;

        if (physMemSize < memSizeField) {
            if (physMemSize - 0x01800000 == 0) {
                DCInvalidateRange((void*)0x81800000, 0x01800000);
                __MEMRegs[20] = 2;
            }
        }
    }

    if (size <= 0x01800000) {
        RealMode((void*)Config24MB);
    } else if (size <= 0x03000000) {
        RealMode((void*)Config48MB);
    }

    __OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_ADDRESS);

    OSRestoreInterrupts(enabled);
}

#if !defined(OSMEMORY_PRIVILEGED_ONLY)
/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 4 function(s)
 * =================================================================== */

typedef struct OSMessageQueue {
    OSThreadQueue queueSend;
    OSThreadQueue queueReceive;
    u32* msgArray;
    s32 msgCount;
    s32 firstIndex;
    s32 usedCount;
} OSMessageQueue;

/* OSSendMessage - 0x8009F230 | size: 0xC8
 * OSSendMessage - Enqueue a message into an OS message queue.
 * If blocking (flags & 1), waits when full.
 * Returns TRUE on success, FALSE if non-blocking and queue is full.
 */
BOOL OSSendMessage(OSMessageQueue* mq, void* msg, s32 flags) {
    BOOL enabled;
    s32 lastIndex;

    enabled = OSDisableInterrupts();
    while (mq->msgCount <= mq->usedCount) {
        if (!(flags & 1)) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        OSSleepThread(&mq->queueSend);
    }
    lastIndex = (mq->firstIndex + mq->usedCount) % mq->msgCount;
    mq->msgArray[lastIndex] = (u32)msg;
    mq->usedCount++;
    OSWakeupThread(&mq->queueReceive);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* OSReceiveMessage - 0x8009F2F8 | size: 0xDC
 * OSReceiveMessage - Dequeue a message from an OS message queue.
 * If msg is non-NULL, stores the dequeued message there.
 * If blocking (flags & 1), waits when empty.
 * Returns TRUE on success, FALSE if non-blocking and queue is empty.
 */
BOOL OSReceiveMessage(OSMessageQueue* mq, void* msg, s32 flags) {
    BOOL enabled = OSDisableInterrupts();

    while (mq->usedCount == 0) {
        if (!(flags & 1)) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        OSSleepThread(&mq->queueReceive);
    }
    if (msg != NULL) {
        *(u32*)msg = mq->msgArray[mq->firstIndex];
    }

    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;
    OSWakeupThread(&mq->queueSend);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* OSGetConsoleSimulatedMemSize - 0x8009F3D4 | size: 0xC
 * OSGetConsoleSimulatedMemSize - returns simulated memory size from boot info.
 */
u32 OSGetConsoleSimulatedMemSize(void) {
    return *(volatile u32*)0x80000028;
}

/* OnReset_800AF628 - 0x8009F3E0 | size: 0x3C
 * OSProtectMemory reset callback - disables memory protection
 * when the system is being reset (final == TRUE).
 * Returns TRUE always.
 */
BOOL OnReset_800AF628(BOOL final) {
    if (final != FALSE) {
        __MEMRegs[8] = 0xFF;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_RESET);
    }
    return TRUE;
}
#endif /* !OSMEMORY_PRIVILEGED_ONLY */
