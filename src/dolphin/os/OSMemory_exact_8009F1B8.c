#include "dolphin/os/OSMemory.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

/*
 * Natural-C prefix of the SDK module/message/memory sequence at
 * 0x8009F1B8 - 0x8009F54C.  The following privileged BAT setup routines
 * remain candidate-only in OSMemory_privileged.c because they require
 * architecture-specific assembly.
 */

#define TRUNC(n, a) (((u32)(n)) & ~((a)-1))
#define ROUND(n, a) (((u32)(n) + (a)-1) & ~((a)-1))

volatile u16 __MEMRegs[64] : (0xCC004000);

#define OS_PROTECT_CONTROL_RDWR 0x03

#define __OS_INTERRUPT_MEM_0 0
#define __OS_INTERRUPT_MEM_1 1
#define __OS_INTERRUPT_MEM_2 2
#define __OS_INTERRUPT_MEM_3 3

#define OS_INTERRUPTMASK(interrupt) (0x80000000u >> (interrupt))
#define OS_INTERRUPTMASK_MEM_0 OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0)
#define OS_INTERRUPTMASK_MEM_1 OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_1)
#define OS_INTERRUPTMASK_MEM_2 OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_2)
#define OS_INTERRUPTMASK_MEM_3 OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_3)
#define OS_INTERRUPTMASK_MEM_RESET \
    (OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 | OS_INTERRUPTMASK_MEM_3)

extern OSErrorHandler __OSErrorTable[];

typedef struct OSModuleInfo OSModuleInfo;
typedef struct OSModuleQueue {
    OSModuleInfo* head;
    OSModuleInfo* tail;
} OSModuleQueue;

typedef struct OSMessageQueue {
    OSThreadQueue queueSend;
    OSThreadQueue queueReceive;
    u32* msgArray;
    s32 msgCount;
    s32 firstIndex;
    s32 usedCount;
} OSMessageQueue;

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

u32 OSGetConsoleSimulatedMemSize(void) {
    return *(volatile u32*)0x80000028;
}

BOOL OnReset_800AF628(BOOL final) {
    if (final != FALSE) {
        __MEMRegs[8] = 0xFF;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_RESET);
    }
    return TRUE;
}

/*
 * The split tail still refers to this formerly TU-local handler. dtk
 * address-qualifies that cross-object relocation as
 * MEMIntrruptHandler_8009F41C, so the definition must use the same name.
 */
void MEMIntrruptHandler_8009F41C(s16 interrupt, OSContext* context) {
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

    if (control != OS_PROTECT_CONTROL_RDWR) {
        __OSUnmaskInterrupts(OS_INTERRUPTMASK(__OS_INTERRUPT_MEM_0 + chan));
    }

    OSRestoreInterrupts(enabled);
}
