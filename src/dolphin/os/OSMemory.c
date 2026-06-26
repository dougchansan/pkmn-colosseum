#include "dolphin/os/OSMemory.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSReset.h"

/*
 * OSMemory.c - Memory protection and BAT configuration.
 *
 * Sets up the memory protection registers, BAT registers for 24MB/48MB
 * memory configurations, and the memory protection interrupt handler.
 *
 * Matches: 0x8009F1B8 - 0x8009F77C
 */

/* Hardware registers */
#define MI_BASE     ((volatile u16*)0xCC004000)
#define MI_MARR_HI  (*(volatile u16*)0xCC00401E)
#define MI_MARR_LO  (*(volatile u16*)0xCC004020)
#define MI_MARR_CTL (*(volatile u16*)0xCC004022)
#define MI_PROT     (*(volatile u16*)0xCC004010)
#define MI_INTMSK   (*(volatile u16*)0xCC004028)

/* Error table for memory protection */
extern OSErrorHandler __OSErrorTable[];

/* Reset function info for memory protection */
extern OSResetFunctionInfo ResetFunctionInfo;

static void MEMIntrruptHandler(s16 interrupt, OSContext* context);
static void Config24MB(void);
static void Config48MB(void);
static void RealMode(void* target);

void __OSModuleInit(void) {
    volatile u32* bootInfo = (volatile u32*)0x80000000;
    bootInfo[0x30CC / 4] = 0;
    bootInfo[0x30C8 / 4] = 0;
    bootInfo[0x30D0 / 4] = 0;
}

extern void OSInitThreadQueue();
void fn_8009F1D0(u8* ptr, u32 val1, u32 val2) {
    OSInitThreadQueue(ptr);
    OSInitThreadQueue(ptr + 0x8);
    *(u32*)(ptr + 0x10) = val1;
    *(u32*)(ptr + 0x14) = val2;
    *(u32*)(ptr + 0x18) = 0;
    *(u32*)(ptr + 0x1C) = 0;
}

static void MEMIntrruptHandler(s16 interrupt, OSContext* context) {
    volatile u16* mi = (volatile u16*)0xCC004000;
    u32 cause;
    u16 hi, lo;

    hi  = mi[0x24 / 2];
    lo  = mi[0x22 / 2];

    /* Combine address */
    cause = ((u32)hi << 16) | lo;

    /* Clear the interrupt */
    mi[0x20 / 2] = 0;

    if (__OSErrorTable[OS_ERROR_PROTECTION] != NULL) {
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, cause, 0);
    } else {
        __OSUnhandledException(OS_ERROR_PROTECTION, context, 0, 0);
    }
}

#if 1
asm void fn_8009F488(void) {
#include "src/dolphin/os/OSMemory_fn_8009F488.inc"
}
#else
void fn_8009F488(void) { /* TODO */ }
#endif

/* Config24MB - sets BAT registers for 24MB physical memory layout */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
static asm void Config24MB(void) {
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
#pragma pop

/* Config48MB - sets BAT registers for 48MB physical memory layout */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
static asm void Config48MB(void) {
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
#pragma pop

/* RealMode - enter real mode (disable address translation) then jump to target */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
static asm void RealMode(register void* target) {
    nofralloc
    clrlwi  r3, r3, 2         /* mask to physical address */
    mtsrr0  r3
    mfmsr   r3
    rlwinm  r3, r3, 0, 28, 25 /* clear IR, DR bits */
    mtsrr1  r3
    rfi
}
#pragma pop

void __OSInitMemoryProtection(void) {
    BOOL enabled;
    u32  memSize;
    volatile u16* mi = (volatile u16*)0xCC004000;

    memSize = *(volatile u32*)0x800000F0;

    enabled = OSDisableInterrupts();

    /* Clear protection registers */
    mi[0x20 / 2] = 0;
    mi[0x10 / 2] = 0x00FF;

    /* Mask all memory protection interrupts initially */
    __OSMaskInterrupts(0xF0000000);

    /* Install memory interrupt handler for all 5 channels */
    __OSSetInterruptHandler(0, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(1, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(2, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(3, (__OSInterruptHandler)MEMIntrruptHandler);
    __OSSetInterruptHandler(4, (__OSInterruptHandler)MEMIntrruptHandler);

    /* Register reset function */
    OSRegisterResetFunction(&ResetFunctionInfo);

    /* Check for extended memory and configure BATs */
    {
        u32 physMemSize = *(volatile u32*)0x800000F0;
        u32 memSizeField = *(volatile u32*)0x80000028;

        if (physMemSize < memSizeField) {
            /* Check for 24MB expansion */
            if (physMemSize - 0x01800000 == 0) {
                DCInvalidateRange((void*)0x81800000, 0x01800000);
                mi[0x28 / 2] = 2;
            }
        }
    }

    /* Set BAT registers based on physical memory size */
    if (memSize <= 0x01800000) {
        /* 24 MB or less */
        RealMode((void*)Config24MB);
    } else if (memSize <= 0x03000000) {
        /* 48 MB or less */
        RealMode((void*)Config48MB);
    }

    /* Unmask the memory protection interrupt */
    __OSUnmaskInterrupts(0x00000800);

    OSRestoreInterrupts(enabled);
}

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 4 function(s)
 * =================================================================== */

/* fn_8009F230 - 0x8009F230 | size: 0xC8
 * OSEnqueueMessage - Enqueue a message into an OS message queue.
 * queue+0x10 = buffer, queue+0x14 = capacity, queue+0x18 = head,
 * queue+0x1C = count. If blocking (flags & 1), waits when full.
 * Returns TRUE on success, FALSE if non-blocking and queue is full.
 */
BOOL fn_8009F230(u8* queue, u32 msg, u32 flags) {
    typedef struct OSMessageQueue {
        u8 sendQueue[0x8];
        u8 recvQueue[0x8];
        u32* buffer;
        s32 capacity;
        s32 head;
        s32 count;
    } OSMessageQueue;
    extern void fn_800A238C(u8* queue);
    extern void fn_800A2478(u8* queue);
    OSMessageQueue* mq;
    s32 blocking;
    BOOL enabled;

    mq = (OSMessageQueue*)queue;
    blocking = flags;
    enabled = OSDisableInterrupts();
    blocking = blocking & 1;

    while (mq->capacity <= mq->count) {
        if (blocking == 0) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        fn_800A238C(queue);
    }

    mq->buffer[(mq->head + mq->count) % mq->capacity] = msg;
    mq->count++;

    fn_800A2478(mq->recvQueue);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* fn_8009F2F8 - 0x8009F2F8 | size: 0xDC
 * OSReceiveMessage - Dequeue a message from an OS message queue.
 * If msgOut is non-NULL, stores the dequeued message there.
 * If blocking (flags & 1), waits when empty.
 * Returns TRUE on success, FALSE if non-blocking and queue is empty.
 */
BOOL fn_8009F2F8(u8* queue, u32* msgOut, u32 flags) {
    extern void fn_800A238C(u8* condvar);
    extern void fn_800A2478(u8* queue);
    BOOL enabled;
    u32 count;
    u32 head;
    u32 capacity;

    flags = flags & 1;
    enabled = OSDisableInterrupts();

    while (1) {
        count = *(u32*)(queue + 0x1C);
        if ((s32)count != 0) {
            break;
        }
        if (flags == 0) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        fn_800A238C(queue + 0x8);
    }

    if (msgOut != NULL) {
        u32* buf;
        head = *(u32*)(queue + 0x18);
        buf = (u32*)(*(u32*)(queue + 0x10));
        *msgOut = buf[head];
    }

    /* Advance head pointer with wraparound */
    head = *(u32*)(queue + 0x18);
    capacity = *(u32*)(queue + 0x14);
    head = (head + 1) % capacity;
    *(u32*)(queue + 0x18) = head;
    *(u32*)(queue + 0x1C) = head;  /* Note: original stores head back to count field */
    fn_800A2478(queue);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* fn_8009F3D4 - 0x8009F3D4 | size: 0xC
 * OSGetConsoleSimulatedMemSize - returns simulated memory size from boot info.
 */
u32 fn_8009F3D4(void) {
    return *(volatile u32*)0x80000028;
}

/* fn_8009F3E0 - 0x8009F3E0 | size: 0x3C
 * OSProtectMemory reset callback - disables memory protection
 * when the system is being reset (final == TRUE).
 * Returns TRUE always.
 */
BOOL fn_8009F3E0(s32 final) {
    if (final != 0) {
        *(volatile u16*)0xCC004010 = 0xFF;
        __OSMaskInterrupts(0xF0000000);
    }
    return TRUE;
}
