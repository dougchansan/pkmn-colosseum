#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/OSTime.h"

typedef u32 OSInterruptMask;

#define OS_INTERRUPTMASK(interrupt) (0x80000000u >> (interrupt))

#define OS_INTERRUPTMASK_MEM_0       OS_INTERRUPTMASK(0)
#define OS_INTERRUPTMASK_MEM_1       OS_INTERRUPTMASK(1)
#define OS_INTERRUPTMASK_MEM_2       OS_INTERRUPTMASK(2)
#define OS_INTERRUPTMASK_MEM_3       OS_INTERRUPTMASK(3)
#define OS_INTERRUPTMASK_MEM_ADDRESS OS_INTERRUPTMASK(4)
#define OS_INTERRUPTMASK_DSP_AI      OS_INTERRUPTMASK(5)
#define OS_INTERRUPTMASK_DSP_ARAM    OS_INTERRUPTMASK(6)
#define OS_INTERRUPTMASK_DSP_DSP     OS_INTERRUPTMASK(7)
#define OS_INTERRUPTMASK_AI_AI       OS_INTERRUPTMASK(8)
#define OS_INTERRUPTMASK_EXI_0_EXI   OS_INTERRUPTMASK(9)
#define OS_INTERRUPTMASK_EXI_0_TC    OS_INTERRUPTMASK(10)
#define OS_INTERRUPTMASK_EXI_0_EXT   OS_INTERRUPTMASK(11)
#define OS_INTERRUPTMASK_EXI_1_EXI   OS_INTERRUPTMASK(12)
#define OS_INTERRUPTMASK_EXI_1_TC    OS_INTERRUPTMASK(13)
#define OS_INTERRUPTMASK_EXI_1_EXT   OS_INTERRUPTMASK(14)
#define OS_INTERRUPTMASK_EXI_2_EXI   OS_INTERRUPTMASK(15)
#define OS_INTERRUPTMASK_EXI_2_TC    OS_INTERRUPTMASK(16)
#define OS_INTERRUPTMASK_PI_CP       OS_INTERRUPTMASK(17)
#define OS_INTERRUPTMASK_PI_PE_TOKEN OS_INTERRUPTMASK(18)
#define OS_INTERRUPTMASK_PI_PE_FINISH OS_INTERRUPTMASK(19)
#define OS_INTERRUPTMASK_PI_SI       OS_INTERRUPTMASK(20)
#define OS_INTERRUPTMASK_PI_DI       OS_INTERRUPTMASK(21)
#define OS_INTERRUPTMASK_PI_RSW      OS_INTERRUPTMASK(22)
#define OS_INTERRUPTMASK_PI_ERROR    OS_INTERRUPTMASK(23)
#define OS_INTERRUPTMASK_PI_VI       OS_INTERRUPTMASK(24)
#define OS_INTERRUPTMASK_PI_DEBUG    OS_INTERRUPTMASK(25)
#define OS_INTERRUPTMASK_PI_HSP      OS_INTERRUPTMASK(26)

#define OS_INTERRUPTMASK_MEM \
    (OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | \
     OS_INTERRUPTMASK_MEM_2 | OS_INTERRUPTMASK_MEM_3 | \
     OS_INTERRUPTMASK_MEM_ADDRESS)
#define OS_INTERRUPTMASK_PI_PE \
    (OS_INTERRUPTMASK_PI_PE_TOKEN | OS_INTERRUPTMASK_PI_PE_FINISH)
#define OS_INTERRUPTMASK_AI OS_INTERRUPTMASK_AI_AI
#define OS_INTERRUPTMASK_EXI \
    (OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_0_TC | \
     OS_INTERRUPTMASK_EXI_0_EXT | OS_INTERRUPTMASK_EXI_1_EXI | \
     OS_INTERRUPTMASK_EXI_1_TC | OS_INTERRUPTMASK_EXI_1_EXT | \
     OS_INTERRUPTMASK_EXI_2_EXI | OS_INTERRUPTMASK_EXI_2_TC)

#define __PIRegs  ((volatile u32*)0xCC003000)
#define __MEMRegs ((volatile u16*)0xCC004000)
#define __DSPRegs ((volatile u16*)0xCC005000)
#define __AIRegs  ((volatile u32*)0xCC006C00)
#define __EXIRegs ((volatile u32*)0xCC006800)

static OSInterruptMask InterruptPrioTable[] = {
    OS_INTERRUPTMASK_PI_ERROR,
    OS_INTERRUPTMASK_PI_DEBUG,
    OS_INTERRUPTMASK_MEM,
    OS_INTERRUPTMASK_PI_RSW,
    OS_INTERRUPTMASK_PI_VI,
    OS_INTERRUPTMASK_PI_PE,
    OS_INTERRUPTMASK_PI_HSP,
    OS_INTERRUPTMASK_DSP_ARAM | OS_INTERRUPTMASK_DSP_DSP |
        OS_INTERRUPTMASK_AI | OS_INTERRUPTMASK_EXI |
        OS_INTERRUPTMASK_PI_SI | OS_INTERRUPTMASK_PI_DI,
    OS_INTERRUPTMASK_DSP_AI,
    OS_INTERRUPTMASK_PI_CP,
    0xFFFFFFFF,
};

extern volatile __OSInterrupt __OSLastInterrupt;
extern volatile OSTime __OSLastInterruptTime;
extern volatile u32 __OSLastInterruptSrr0;

void __OSDispatchInterrupt(__OSException exception, OSContext* context)
{
    u32 intsr;
    u32 reg;
    OSInterruptMask cause;
    OSInterruptMask unmasked;
    OSInterruptMask* prio;
    __OSInterrupt interrupt;
    __OSInterruptHandler handler;

    (void)exception;
    intsr = __PIRegs[0];
    intsr &= ~0x00010000;

    if (intsr == 0 || (intsr & __PIRegs[1]) == 0) {
        OSLoadContext(context);
    }

    cause = 0;

    if (intsr & 0x00000080) {
        reg = __MEMRegs[15];
        if (reg & 0x1) cause |= OS_INTERRUPTMASK_MEM_0;
        if (reg & 0x2) cause |= OS_INTERRUPTMASK_MEM_1;
        if (reg & 0x4) cause |= OS_INTERRUPTMASK_MEM_2;
        if (reg & 0x8) cause |= OS_INTERRUPTMASK_MEM_3;
        if (reg & 0x10) cause |= OS_INTERRUPTMASK_MEM_ADDRESS;
    }

    if (intsr & 0x00000040) {
        reg = __DSPRegs[5];
        if (reg & 0x8) cause |= OS_INTERRUPTMASK_DSP_AI;
        if (reg & 0x20) cause |= OS_INTERRUPTMASK_DSP_ARAM;
        if (reg & 0x80) cause |= OS_INTERRUPTMASK_DSP_DSP;
    }

    if (intsr & 0x00000020) {
        reg = __AIRegs[0];
        if (reg & 0x8) cause |= OS_INTERRUPTMASK_AI_AI;
    }

    if (intsr & 0x00000010) {
        reg = __EXIRegs[0];
        if (reg & 0x2) cause |= OS_INTERRUPTMASK_EXI_0_EXI;
        if (reg & 0x8) cause |= OS_INTERRUPTMASK_EXI_0_TC;
        if (reg & 0x800) cause |= OS_INTERRUPTMASK_EXI_0_EXT;
        reg = __EXIRegs[5];
        if (reg & 0x2) cause |= OS_INTERRUPTMASK_EXI_1_EXI;
        if (reg & 0x8) cause |= OS_INTERRUPTMASK_EXI_1_TC;
        if (reg & 0x800) cause |= OS_INTERRUPTMASK_EXI_1_EXT;
        reg = __EXIRegs[10];
        if (reg & 0x2) cause |= OS_INTERRUPTMASK_EXI_2_EXI;
        if (reg & 0x8) cause |= OS_INTERRUPTMASK_EXI_2_TC;
    }

    if (intsr & 0x00002000) cause |= OS_INTERRUPTMASK_PI_HSP;
    if (intsr & 0x00001000) cause |= OS_INTERRUPTMASK_PI_DEBUG;
    if (intsr & 0x00000400) cause |= OS_INTERRUPTMASK_PI_PE_FINISH;
    if (intsr & 0x00000200) cause |= OS_INTERRUPTMASK_PI_PE_TOKEN;
    if (intsr & 0x00000100) cause |= OS_INTERRUPTMASK_PI_VI;
    if (intsr & 0x00000008) cause |= OS_INTERRUPTMASK_PI_SI;
    if (intsr & 0x00000004) cause |= OS_INTERRUPTMASK_PI_DI;
    if (intsr & 0x00000002) cause |= OS_INTERRUPTMASK_PI_RSW;
    if (intsr & 0x00000800) cause |= OS_INTERRUPTMASK_PI_CP;
    if (intsr & 0x00000001) cause |= OS_INTERRUPTMASK_PI_ERROR;

    unmasked = cause & ~(*(OSInterruptMask*)0x800000C4 |
                         *(OSInterruptMask*)0x800000C8);
    if (unmasked) {
        for (prio = InterruptPrioTable;; ++prio) {
            if (unmasked & *prio) {
                interrupt = (__OSInterrupt)__cntlzw(unmasked & *prio);
                break;
            }
        }

        handler = __OSGetInterruptHandler(interrupt);
        if (handler) {
            if (4 < interrupt) {
                __OSLastInterrupt = interrupt;
                __OSLastInterruptTime = OSGetTime();
                __OSLastInterruptSrr0 = context->srr0;
            }
            OSDisableScheduler();
            handler(interrupt, context);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }
    }

    OSLoadContext(context);
}
