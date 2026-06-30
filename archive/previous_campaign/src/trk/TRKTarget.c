#include "dolphin/types.h"

/*
 * TRKTarget.c - TRK target control functions.
 *
 * Manages the stopped/running state of the target program,
 * handles interrupt events, and processes support requests
 * (system calls from the target via trap instructions).
 */

extern void MWTRACE(s32 level, const char* fmt, ...);
extern void fn_800BE464(void* event, s32 type);
extern s32  fn_800BE47C(void* event);
extern s32  fn_800C0CD8(s32 event);
extern s32  fn_800C07A4(u32 addr, u8 type, u32 count, void* result);
extern s32  fn_800C06BC(u32 addr, void* result);
extern s32  fn_800C05AC(u32 addr, void* data, u8 type, void* result, void* length);
extern s32  fn_800C0AA0(u32 addr, u32 count, void* buf, void* result, u32 flags, u32 isD1, u32 step);
extern void fn_800C0D70(u32 addr, u32 size);
extern s32  fn_800C25B0(u8* buf, u32 pc);

extern void TRKTargetSetStopped(s32 stopped);
extern void UnreserveEXI2Port(void);
extern void ReserveEXI2Port(void);
extern void TRKSwapAndGo(void);

/* TRK state and CPU state structures */
extern u8 gTRKState[];     /* large state structure */
extern u8 gTRKCPUState[];  /* saved CPU context */

/* Breakpoint info at lbl_80313834 */
extern u8 lbl_80313834[];

/* TRK exception status structure at 80313824 */
extern u8 gTRKExceptionStatus_80313824[];

/* TRK restore flags */
extern u8 gTRKRestoreFlags[];

/*
 * TRKTargetSetInputPendingPtr - Store the input pending flag pointer.
 * This pointer is checked by interrupt handlers to determine if
 * there is pending serial input.
 */
void TRKTargetSetInputPendingPtr(u8* ptr) {
    /* gTRKState offset 0xA0 = inputPendingPtr */
    *(u32*)&gTRKState[0xA0] = (u32)ptr;
}

/*
 * TRKTargetSetStopped - Set the target stopped flag.
 * 0 = running, 1 = stopped.
 */
void TRKTargetSetStopped(s32 stopped) {
    /* gTRKState offset 0x98 = stopped flag */
    *(s32*)&gTRKState[0x98] = stopped;
}

/*
 * TRKTargetStopped - Query whether the target is stopped.
 * Returns 1 if stopped, 0 if running.
 */
s32 TRKTargetStopped(void) {
    return *(s32*)&gTRKState[0x98];
}

/*
 * TRKTargetSupportRequest - Handle a system call from the target.
 *
 * The target communicates with the debugger by executing special
 * trap instructions. The "reason" code in GPR3 (offset 0x0C in
 * the saved CPU state) determines what operation is requested:
 *   0xD0, 0xD1: Memory read/write with step
 *   0xD2: Read from address
 *   0xD3: Open file
 *   0xD4: Write to file
 */
void TRKTargetSupportRequest(void) {
    u8* cpuState = gTRKCPUState;
    s32 reason;
    s32 result;
    s32 err;

    reason = *(s32*)&cpuState[0x0C]; /* GPR3 = reason code */

    /* If not a recognized support request, post a "stop" event */
    if (reason != 0xD1 && reason != 0xD0 && reason != 0xD2 &&
        reason != 0xD3 && reason != 0xD4) {
        u8 eventBuf[0x10];
        fn_800BE464((void*)eventBuf, 4);
        fn_800BE47C((void*)eventBuf);
        return;
    }

    if (reason == 0xD2) {
        /* Read memory request */
        u32 addr = *(u32*)&cpuState[0x10]; /* GPR4 = address */
        u8  type = (u8)(*(u32*)&cpuState[0x14]); /* GPR5 = type */
        u32 count = *(u32*)&cpuState[0x18]; /* GPR6 = count */
        u32 resultCode = 0;

        err = fn_800C07A4(addr, type, count, &resultCode);

        if (resultCode == 0 && err != 0) {
            resultCode = 1;
        }
        *(u32*)&cpuState[0x0C] = resultCode;
    } else if (reason == 0xD3) {
        /* File open request */
        u32 addr = *(u32*)&cpuState[0x10]; /* GPR4 = filename addr */
        u32 resultCode = 0;

        err = fn_800C06BC(addr, &resultCode);

        if (resultCode == 0 && err != 0) {
            resultCode = 1;
        }
        *(u32*)&cpuState[0x0C] = resultCode;
    } else if (reason == 0xD4) {
        /* File write request */
        u32 ptrAddr = *(u32*)&cpuState[0x14]; /* GPR5 = data pointer addr */
        u32 count = *(u32*)&cpuState[0x18]; /* GPR6 = count */
        u32 data = *(u32*)ptrAddr;
        u32 addr = *(u32*)&cpuState[0x10]; /* GPR4 = file handle */
        u8  type = (u8)count;
        u32 resultCode = 0;

        err = fn_800C05AC(addr, &data, type, &resultCode, &data);

        if (resultCode == 0 && err != 0) {
            resultCode = 1;
        }
        *(u32*)&cpuState[0x0C] = resultCode;
        *(u32*)ptrAddr = data;
    } else {
        /* 0xD0 or 0xD1: generic memory access */
        u32 addr = *(u32*)&cpuState[0x10]; /* GPR4 */
        u32 count = *(u32*)&cpuState[0x18]; /* GPR6 */
        u32* buf = (u32*)&cpuState[0x14]; /* GPR5 = buffer */
        u32 isD1 = (reason == 0xD1) ? 1 : 0;
        u32 resultCode = 0;

        err = fn_800C0AA0(addr, count, (void*)buf, &resultCode, (u32)*buf, isD1, 1);

        if (resultCode == 0 && err != 0) {
            resultCode = 1;
        }
        *(u32*)&cpuState[0x0C] = resultCode;

        if (reason == 0xD1) {
            fn_800C0D70(count, *(u32*)buf);
        }
    }

    /* Advance PC past the trap instruction */
    {
        u32 pc = *(u32*)&cpuState[0x80]; /* SRR0/PC */
        *(u32*)&cpuState[0x80] = pc + 4;
    }
}

/*
 * TRKTargetInterrupt - Handle a debugger interrupt event.
 *
 * Checks if the interrupt should cause a stop based on breakpoint
 * configuration. If a breakpoint match is found, sets the target
 * as stopped and signals a break event.
 */
s32 TRKTargetInterrupt(void* event) {
    s32 result = 0;
    s32 eventType = *(s32*)event;
    s32 needCheckStop = 0;

    /* Only handle types 3 and 4 */
    if (eventType >= 3 && eventType < 5) {
        u8* bpInfo = lbl_80313834;
        s32 bpActive = *(s32*)&bpInfo[0];

        if (bpActive == 0) {
            needCheckStop = 1;
        } else {
            u8* cpuState = gTRKCPUState;
            s32 doStop = 1;
            u32 msr;

            /* Clear single-step bit in MSR (bit 10) */
            msr = *(u32*)&cpuState[0x1F8];
            msr &= ~0x400;
            *(u32*)&cpuState[0x1F8] = msr;

            /* Check if this is a trace exception (0xD00) */
            {
                u32 excID = *(u32*)&cpuState[0x2F8] & 0xFFFF;
                if (excID == 0xD00) {
                    s32 bpType = *(s32*)&bpInfo[4];

                    if (bpType == 1) {
                        /* Range breakpoint: check if PC is in range */
                        u32 pc = *(u32*)&cpuState[0x80];
                        u32 rangeStart = *(u32*)&bpInfo[0x0C];
                        u32 rangeEnd = *(u32*)&bpInfo[0x10];

                        if (pc >= rangeStart && pc <= rangeEnd) {
                            doStop = 0;
                        }
                    } else if (bpType == 0) {
                        /* Count breakpoint */
                        u32 count = *(u32*)&bpInfo[8];
                        if (count != 0) {
                            doStop = 0;
                        }
                    }
                }
            }

            if (doStop != 0) {
                /* Clear breakpoint and go to check_stop */
                *(s32*)&bpInfo[0] = 0;
                needCheckStop = 1;
            } else {
                /* check_stop_2: Breakpoint still active */
                *(s32*)&bpInfo[0] = 1;

                MWTRACE(1, "TRKTargetInterrupt: stepping\n");

                /* Re-enable single-step bit */
                msr = *(u32*)&cpuState[0x1F8];
                msr |= 0x400;
                *(u32*)&cpuState[0x1F8] = msr;

                {
                    s32 bpType2 = *(s32*)&bpInfo[4];
                    if (bpType2 == 0 || bpType2 == 0x10) {
                        /* Decrement step count */
                        u32 count2 = *(u32*)&bpInfo[8];
                        *(u32*)&bpInfo[8] = count2 - 1;
                    }
                }

                /* Clear stopped flag */
                *(s32*)&gTRKState[0x98] = 0;
            }
        }

        /* check_stop */
        if (needCheckStop) {
            u8* bpInfo2 = lbl_80313834;
            s32 bpActive2 = *(s32*)&bpInfo2[0];

            if (bpActive2 == 0) {
                /* Set stopped and signal break event */
                *(s32*)&gTRKState[0x98] = 1;
                result = fn_800C0CD8(0x90);
            }
        }
    }

    return result;
}

/*
 * TRKPostInterruptEvent - Determine event type and post to queue.
 *
 * Examines the saved exception ID to determine what kind of event
 * to post: support request (0xD00 trap), break, or generic interrupt.
 * Called from interrupt handler assembly code.
 */
void TRKPostInterruptEvent(void) {
    u8* state = gTRKState;

    /* Check inputNotify flag at offset 0x9C */
    if (*(s32*)&state[0x9C] != 0) {
        *(s32*)&state[0x9C] = 0;
        return;
    }

    {
        u32 excID;
        s32 eventType;
        u8 eventBuf[0x10];
        u8 readBuf[8];

        excID = *(u32*)&gTRKCPUState[0x2F8] & 0xFFFF;

        if (excID == 0xD00 || excID == 0x700) {
            /* Check if this is a support request (trap) */
            u32 pc = *(u32*)&gTRKCPUState[0x80]; /* SRR0 */

            fn_800C25B0(readBuf, pc);

            {
                u32 instr = *(u32*)&readBuf[0];
                /* Check for tw 31,0,0 (0x0FE00000) = support request trap */
                if (instr == 0x0FE00000) {
                    eventType = 5; /* support request */
                } else {
                    eventType = 3; /* break */
                }
            }
        } else {
            eventType = 4; /* generic interrupt */
        }

        fn_800BE464((void*)eventBuf, eventType);
        fn_800BE47C((void*)eventBuf);
    }
}

/*
 * TRKTargetContinue - Resume target execution.
 *
 * Clears the stopped flag, unreserves the EXI port so the target
 * can use it, swaps to target context, then re-reserves the port
 * when the target stops again.
 */
s32 TRKTargetContinue(void) {
    TRKTargetSetStopped(0);
    UnreserveEXI2Port();
    TRKSwapAndGo();
    ReserveEXI2Port();
    return 0;
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800C1310 - 0x800C1310 | size: 0x18 */
s32 fn_800C1310(void) {
    *(s32*)&gTRKState[0x98] = 1;
    return 0;
}

/* fn_800C1548 - 0x800C1548 | size: 0x10 */
s32 fn_800C1548(void) {
    return *(s32*)&gTRKCPUState[0x80];
}

/* TRKTargetStepOutOfRange - 0x800C1558 | size: 0xB8 */
s32 TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, s32 c) {
    extern u8 lbl_8026FB70[];
    u8* bpInfo;
    u8* cpuState;
    s32 bpType;
    u32 msr;

    if (c != 0) {
        return 0x703;
    }
    bpInfo = lbl_80313834;
    *(u32*)&bpInfo[0xC] = rangeStart;
    *(u32*)&bpInfo[0x10] = rangeEnd;
    *(s32*)&bpInfo[0x4] = 1;
    *(s32*)&bpInfo[0x0] = 1;

    MWTRACE(1, (const char*)lbl_8026FB70);

    cpuState = gTRKCPUState;
    bpType = *(s32*)&bpInfo[0x4];
    msr = *(u32*)&cpuState[0x1F8];
    msr |= 0x400;
    *(u32*)&cpuState[0x1F8] = msr;

    if (bpType == 0 || bpType == 0x10) {
        *(u32*)&lbl_80313834[0x8] = *(u32*)&lbl_80313834[0x8] - 1;
    }
    *(s32*)&gTRKState[0x98] = 0;
    return 0;
}

/* TRKTargetSingleStep - 0x800C1610 | size: 0xAC */
s32 TRKTargetSingleStep(u32 count, s32 c) {
    extern u8 lbl_8026FB70[];
    u8* bpInfo;
    u8* cpuState;
    s32 bpType;
    u32 msr;

    if (c != 0) {
        return 0x703;
    }
    bpInfo = lbl_80313834;
    *(u32*)&bpInfo[0x8] = count;
    *(s32*)&bpInfo[0x4] = 0;
    *(s32*)&bpInfo[0x0] = 1;

    MWTRACE(1, (const char*)lbl_8026FB70);

    cpuState = gTRKCPUState;
    bpType = *(s32*)&bpInfo[0x4];
    msr = *(u32*)&cpuState[0x1F8];
    msr |= 0x400;
    *(u32*)&cpuState[0x1F8] = msr;

    if (bpType == 0 || bpType == 0x10) {
        *(u32*)&bpInfo[0x8] = *(u32*)&bpInfo[0x8] - 1;
    }
    *(s32*)&gTRKState[0x98] = 0;
    return 0;
}

/* fn_800C16BC - 0x800C16BC | size: 0x84 */
void fn_800C16BC(s32 arg) {
    extern void fn_800BEBB0(s32 a, void* buf, u32 len);
    u8 buf[0x40];
    s32 result;
    u32 dataword;

    memset(buf, 0, 0x40);
    dataword = *(u32*)gTRKExceptionStatus_80313824;
    *(u32*)&buf[0x0] = 0x40;
    buf[0x4] = 0x91;
    *(u32*)&buf[0x8] = dataword;
    fn_800C25B0((u8*)&result, dataword);
    *(u32*)&buf[0xC] = result;
    *(u32*)&buf[0x10] = *(u16*)&gTRKExceptionStatus_80313824[0x8];
    fn_800BEBB0(arg, buf, 0x40);
}

/* fn_800C1740 - 0x800C1740 | size: 0x8C */
void fn_800C1740(s32 arg) {
    extern void fn_800BEBB0(s32 a, void* buf, u32 len);
    u8 buf[0x40];
    s32 result;
    u32 dataword;

    memset(buf, 0, 0x40);
    dataword = *(u32*)&gTRKCPUState[0x80];
    *(u32*)&buf[0x0] = 0x40;
    buf[0x4] = 0x90;
    *(u32*)&buf[0x8] = dataword;
    fn_800C25B0((u8*)&result, dataword);
    *(u32*)&buf[0xC] = result;
    *(u32*)&buf[0x10] = *(u32*)&gTRKCPUState[0x2F8] & 0xFFFF;
    fn_800BEBB0(arg, buf, 0x40);
}

/* fn_800C1A08 - 0x800C1A08 | size: 0x438 */
void fn_800C1A08(void) {
    extern u8 lbl_8026FAF8[];
    extern u8 lbl_8026FB20[];
    extern u8 lbl_803FED44[];
    extern void fn_800BE9CC();
    extern void fn_800BEC18();
    u8 sp[0x130];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f3 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r23 = r4;
    r27 = r3;
    r24 = r5;
    r25 = r6;
    r26 = r7;
    if (r23 > 0x1f) {
        r3 = 0x701;
        return;
    }
    r3 = (u32)lbl_8026FAF8;
    r5 = (u32)gTRKExceptionStatus_80313824;
    r29 = (u32)lbl_8026FAF8;
    r4 = 0x7C990000;
    tmp = *(u32*)((u8*)r29 + 0x0);
    r31 = (u32)gTRKExceptionStatus_80313824;
    r8 = *(u32*)((u8*)r29 + 0x4);
    r3 = 0x4E800000;
    r7 = *(u32*)((u8*)r29 + 0x24);
    *(u32*)(sp + 0xC4) = tmp;
    tmp = r3 + 0x20;
    r19 = *(u32*)((u8*)r31 + 0x0);
    r30 = 0x0;
    r22 = *(u32*)((u8*)r31 + 0xC);
    r5 = 0x90830000;
    r3 = (u32)sp + 0xc4;
    r20 = *(u32*)((u8*)r31 + 0x4);
    r4 = 0x28;
    r21 = *(u32*)((u8*)r31 + 0x8);
    r28 = *(u32*)((u8*)r29 + 0x8);
    r12 = *(u32*)((u8*)r29 + 0xC);
    r11 = *(u32*)((u8*)r29 + 0x10);
    r10 = *(u32*)((u8*)r29 + 0x14);
    r9 = *(u32*)((u8*)r29 + 0x18);
    r8 = *(u32*)((u8*)r29 + 0x1C);
    r7 = *(u32*)((u8*)r29 + 0x20);
    *(u8*)((u8*)r31 + 0xD) = r30;
    *(u32*)(sp + 0xE8) = tmp;
    ((void(*)(void))fn_800C0D70)();
    r3 = (u32)lbl_803FED44;
    r12 = (u32)sp + 0xc4;
    r4 = (u32)lbl_803FED44;
    r3 = (u32)sp + 0x8;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    r3 = (u32)lbl_8026FAF8;
    r29 = (u32)lbl_8026FAF8;
    r4 = 0x7C990000;
    r8 = *(u32*)((u8*)r29 + 0x0);
    r3 = 0x4E800000;
    tmp = *(u32*)((u8*)r29 + 0x4);
    r30 = r5 | (0xa000 << 16);
    r7 = *(u32*)((u8*)r29 + 0x24);
    *(u32*)(sp + 0xA0) = tmp;
    tmp = r3 + 0x20;
    r28 = *(u32*)((u8*)r29 + 0x8);
    r6 = 0x80830000;
    r3 = (u32)sp + 0x9c;
    r12 = *(u32*)((u8*)r29 + 0xC);
    r4 = 0x28;
    r11 = *(u32*)((u8*)r29 + 0x10);
    r10 = *(u32*)((u8*)r29 + 0x14);
    r9 = *(u32*)((u8*)r29 + 0x18);
    r8 = *(u32*)((u8*)r29 + 0x1C);
    r7 = *(u32*)((u8*)r29 + 0x20);
    *(u32*)(sp + 0xC0) = tmp;
    ((void(*)(void))fn_800C0D70)();
    r3 = (u32)lbl_803FED44;
    r12 = (u32)sp + 0x9c;
    r4 = (u32)lbl_803FED44;
    r3 = (u32)sp + 0x8;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    r3 = (u32)lbl_8026FAF8;
    r4 = 0x7C910000;
    r29 = (u32)lbl_8026FAF8;
    r3 = 0x4E800000;
    r8 = *(u32*)((u8*)r29 + 0x0);
    r30 = 0x0;
    r6 = *(u32*)((u8*)r29 + 0x4);
    r7 = *(u32*)((u8*)r29 + 0x24);
    tmp = r3 + 0x20;
    r6 = 0x80830000;
    r28 = *(u32*)((u8*)r29 + 0x8);
    r3 = (u32)sp + 0x74;
    r4 = 0x28;
    r12 = *(u32*)((u8*)r29 + 0xC);
    r11 = *(u32*)((u8*)r29 + 0x10);
    r10 = *(u32*)((u8*)r29 + 0x14);
    r9 = *(u32*)((u8*)r29 + 0x18);
    r8 = *(u32*)((u8*)r29 + 0x1C);
    r7 = *(u32*)((u8*)r29 + 0x20);
    *(u32*)(sp + 0x98) = tmp;
    ((void(*)(void))fn_800C0D70)();
    r3 = (u32)lbl_803FED44;
    r12 = (u32)sp + 0x74;
    r4 = (u32)lbl_803FED44;
    r3 = (u32)sp + 0x8;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    tmp = 0x0;
    r30 = r27 << 21;
    *(u32*)((u8*)r25 + 0x0) = tmp;
    r29 = (u32)sp + 0x4c;
    r28 = (u32)sp + 0x24;
    r3 = 0x0;
    while (r27 <= r23 && (s32)r3 == 0) {

        if ((s32)r26 != 0) {
            r3 = (u32)lbl_8026FB20;
            r12 = *(u32*)lbl_8026FB20;
            tmp = r30 | (0xe003 << 16);
            r11 = *(u32*)((u8*)r3 + 0x4);
            r10 = *(u32*)((u8*)r3 + 0x8);
            r9 = *(u32*)((u8*)r3 + 0xC);
            r8 = *(u32*)((u8*)r3 + 0x10);
            r7 = *(u32*)((u8*)r3 + 0x14);
            r6 = *(u32*)((u8*)r3 + 0x18);
            r5 = *(u32*)((u8*)r3 + 0x1C);
            r4 = *(u32*)((u8*)r3 + 0x20);
            r3 = *(u32*)((u8*)r3 + 0x24);
            if ((s32)r26 != 0) {
                tmp = r30 | (0xf003 << 16);
            }
            r3 = 0x4E800000;
            *(u32*)(sp + 0x4C) = tmp;
            tmp = r3 + 0x20;
            r3 = r29;
            *(u32*)(sp + 0x70) = tmp;
            r4 = 0x28;
            ((void(*)(void))fn_800C0D70)();
            r3 = (u32)lbl_803FED44;
            r12 = (u32)sp + 0x4c;
            r4 = (u32)lbl_803FED44;
            r3 = (u32)sp + 0xc;
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
            r3 = r24;
            fn_800BEC18();
        } else {

            r3 = r24;
            r4 = (u32)sp + 0xc;
            fn_800BE9CC();
            r3 = (u32)lbl_8026FB20;
            r12 = *(u32*)lbl_8026FB20;
            tmp = r30 | (0xe003 << 16);
            r11 = *(u32*)((u8*)r3 + 0x4);
            r10 = *(u32*)((u8*)r3 + 0x8);
            r9 = *(u32*)((u8*)r3 + 0xC);
            r8 = *(u32*)((u8*)r3 + 0x10);
            r7 = *(u32*)((u8*)r3 + 0x14);
            r6 = *(u32*)((u8*)r3 + 0x18);
            r5 = *(u32*)((u8*)r3 + 0x1C);
            r4 = *(u32*)((u8*)r3 + 0x20);
            r3 = *(u32*)((u8*)r3 + 0x24);
            if ((s32)r26 != 0) {
                tmp = r30 | (0xf003 << 16);
            }
            r3 = 0x4E800000;
            *(u32*)(sp + 0x24) = tmp;
            tmp = r3 + 0x20;
            r3 = r28;
            *(u32*)(sp + 0x48) = tmp;
            r4 = 0x28;
            ((void(*)(void))fn_800C0D70)();
            r3 = (u32)lbl_803FED44;
            r12 = (u32)sp + 0x24;
            r4 = (u32)lbl_803FED44;
            r3 = (u32)sp + 0xc;
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
            r3 = 0x0;
        }
        r4 = *(u32*)((u8*)r25 + 0x0);
        r30 = r30 + (0x20 << 16);
        r27 = r27 + 0x1;
        tmp = r4 + 0x8;
        *(u32*)((u8*)r25 + 0x0) = tmp;


    }

    tmp = *(u8*)((u8*)r31 + 0xD);
    if (tmp != 0) {
        tmp = 0x0;
        r3 = 0x702;
        *(u32*)((u8*)r25 + 0x0) = tmp;
    }
    r4 = (u32)gTRKExceptionStatus_80313824;
    r7 = (u32)gTRKExceptionStatus_80313824;
    *(u32*)((u8*)r7 + 0x0) = r6;
    *(u32*)((u8*)r7 + 0x4) = r5;
    *(u32*)((u8*)r7 + 0x8) = r4;
    *(u32*)((u8*)r7 + 0xC) = tmp;

    return;
}

/* fn_800C1E40 - 0x800C1E40 | size: 0x170 */
void fn_800C1E40(void) {
    extern void fn_800BE844();
    extern void fn_800BEAB4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r6;
    if (r4 > 0x60) {
        r3 = 0x701;
        return;
    }
    r6 = (u32)gTRKExceptionStatus_80313824;
    tmp = 0x0;
    r31 = (u32)gTRKExceptionStatus_80313824;
    r6 = *(u32*)((u8*)r31 + 0xC);
    r10 = *(u32*)((u8*)r31 + 0x0);
    r9 = *(u32*)((u8*)r31 + 0x4);
    r8 = *(u32*)((u8*)r31 + 0x8);
    *(u8*)((u8*)r31 + 0xD) = tmp;
    *(u32*)((u8*)r30 + 0x0) = tmp;
    if (r3 <= r4) {
        r4 = r4 - r3;
        r8 = (u32)gTRKCPUState;
        tmp = r4 + 0x1;
        r4 = *(u32*)((u8*)r30 + 0x0);
        r6 = tmp << 2;
        r4 = r4 + r6;
        r7 = (u32)gTRKCPUState;
        r3 = r3 << 2;
        *(u32*)((u8*)r30 + 0x0) = r4;
        r4 = r7 + r3;
        r4 = r4 + 0x1a8;
        if ((s32)r7 != 0) {
            r3 = r5; r5 = tmp; fn_800BEAB4();
        } else {
            r3 = r7 + 0x1ec;
            if (r4 <= r3) {
                r3 = r7 + 0x1e8; r6 = r4 + r6;
                if (r6 >= r3) { r3 = (u32)gTRKRestoreFlags; r6 = 0x1; *(u8*)gTRKRestoreFlags = r6; }
            }
            r3 = (u32)gTRKCPUState; r3 = (u32)gTRKCPUState; r6 = r3 + 0x278;
            if (r4 <= r6) {
                r3 = tmp << 2; r3 = r4 + r3;
                if (r3 >= r6) { r3 = (u32)gTRKRestoreFlags; r6 = 0x1; r3 = (u32)gTRKRestoreFlags; *(u8*)((u8*)r3 + 0x1) = r6; }
            }
            r3 = r5; r5 = tmp; fn_800BE844();
        }
    }
    tmp = *(u8*)((u8*)r31 + 0xD);
    if (tmp != 0) {
        tmp = 0x0;
        r3 = 0x702;
        *(u32*)((u8*)r30 + 0x0) = tmp;
    }
    r4 = (u32)gTRKExceptionStatus_80313824;
    r7 = (u32)gTRKExceptionStatus_80313824;
    *(u32*)((u8*)r7 + 0x0) = r6;
    *(u32*)((u8*)r7 + 0x4) = r5;
    *(u32*)((u8*)r7 + 0x8) = r4;
    *(u32*)((u8*)r7 + 0xC) = tmp;

    return;
}

/* fn_800C1FB0 - 0x800C1FB0 | size: 0x50C */
void fn_800C1FB0(void) {
    extern u8 lbl_8026FAF8[];
    extern u8 lbl_8026FB48[];
    extern u8 lbl_803FED44[];
    extern void fn_800BE9CC();
    extern void fn_800BEC18();
    extern void fn_800C0E60();
    extern void fn_800C0E68();
    extern void fn_800C11F4();
    extern void fn_800C1218();
    u8 sp[0xF0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r28 = r4;
    r20 = r3;
    r29 = r5;
    r30 = r6;
    r31 = r7;
    if (r28 > 0x21) {
        r3 = 0x701;
        return;
    }
    r3 = (u32)gTRKExceptionStatus_80313824;
    tmp = 0x0;
    r27 = (u32)gTRKExceptionStatus_80313824;
    r3 = *(u32*)((u8*)r27 + 0xC);
    r6 = *(u32*)((u8*)r27 + 0x0);
    r5 = *(u32*)((u8*)r27 + 0x4);
    r4 = *(u32*)((u8*)r27 + 0x8);
    *(u8*)((u8*)r27 + 0xD) = tmp;
    fn_800C0E60();
    r3 = r3 | 0x2000;
    fn_800C0E68();
    tmp = 0x0;
    r21 = r20;
    *(u32*)((u8*)r30 + 0x0) = tmp;
    r26 = r20 << 21;
    r25 = (u32)sp + 0x98;
    r24 = (u32)sp + 0x48;
    r23 = (u32)sp + 0x70;
    r22 = (u32)sp + 0x20;
    r3 = 0x0;
    while (r21 <= r28 && (s32)r3 == 0) {

        if ((s32)r31 != 0) {
            r3 = (u32)lbl_8026FB48;
            r12 = (u32)lbl_8026FB48;
            r11 = *(u32*)((u8*)r12 + 0x0);
            r10 = *(u32*)((u8*)r12 + 0x4);
            r9 = *(u32*)((u8*)r12 + 0x8);
            r8 = *(u32*)((u8*)r12 + 0xC);
            r7 = *(u32*)((u8*)r12 + 0x10);
            r6 = *(u32*)((u8*)r12 + 0x14);
            r5 = *(u32*)((u8*)r12 + 0x18);
            r4 = *(u32*)((u8*)r12 + 0x1C);
            r3 = *(u32*)((u8*)r12 + 0x20);
            tmp = *(u32*)((u8*)r12 + 0x24);
            *(u32*)(sp + 0xBC) = tmp;
            if (r21 < 0x20) {
                tmp = r26 | (0xc803 << 16);
                if ((s32)r31 != 0) {
                    tmp = r26 | (0xd803 << 16);
                }
                r3 = 0x4E800000;
                *(u32*)(sp + 0x98) = tmp;
                tmp = r3 + 0x20;
                r3 = r25;
                *(u32*)(sp + 0xBC) = tmp;
                r4 = 0x28;
                ((void(*)(void))fn_800C0D70)();
                r3 = (u32)lbl_803FED44;
                r12 = (u32)sp + 0x98;
                r4 = (u32)lbl_803FED44;
                r3 = (u32)sp + 0x8;
                ctr_fn = (void(*)(void))r12;
                ctr_fn();

            } else if ((s32)r31 == 0) {
                if ((s32)r31 != 0) {
                    r3 = (u32)sp + 0x8;
                    fn_800C11F4();
                } else {

                    r3 = (u32)sp + 0x8;
                    fn_800C1218();
                }
                tmp = 0x0;
                r3 = -0x1;
                tmp = r4 & tmp;
                r3 = r5 & r3;
                *(u32*)(sp + 0x8) = tmp;

            } else {
                if (r21 == 0x21) {
                    if ((s32)r31 == 0) { *(u32*)(sp + 0x8) = tmp; }
                    r3 = (u32)lbl_8026FAF8; r12 = (u32)lbl_8026FAF8;
                    r11 = *(u32*)((u8*)r12 + 0x0); r10 = *(u32*)((u8*)r12 + 0x4);
                    r9 = *(u32*)((u8*)r12 + 0x8); r8 = *(u32*)((u8*)r12 + 0xC);
                    r7 = *(u32*)((u8*)r12 + 0x10); r6 = *(u32*)((u8*)r12 + 0x14);
                    r5 = *(u32*)((u8*)r12 + 0x18); r4 = *(u32*)((u8*)r12 + 0x1C);
                    r3 = *(u32*)((u8*)r12 + 0x20); tmp = *(u32*)((u8*)r12 + 0x24);
                    *(u32*)(sp + 0x6C) = tmp;
                    if ((s32)r31 != 0) { r3 = 0x7C9F0000; tmp = 0x90830000; *(u32*)(sp + 0x4C) = tmp; }
                    else { r3 = 0x7C9F0000; r4 = 0x80830000; *(u32*)(sp + 0x4C) = tmp; }
                    r4 = 0x4E800000; r3 = r24; tmp = r4 + 0x20; r4 = 0x28;
                    *(u32*)(sp + 0x6C) = tmp; ((void(*)(void))fn_800C0D70)();
                    r3 = (u32)lbl_803FED44; r12 = (u32)sp + 0x48;
                    r4 = (u32)lbl_803FED44; r3 = (u32)sp + 0x8;
                    ctr_fn = (void(*)(void))r12; ctr_fn();
                    if ((s32)r31 != 0) { r4 = 0x0; tmp = -0x1; r3 = r3 & tmp; tmp = r4 & r4; *(u32*)(sp + 0x8) = tmp; }
                }
            }
            r3 = r29;
            fn_800BEC18();
        } else {

            r3 = r29;
            r4 = (u32)sp + 0x8;
            fn_800BE9CC();
            r3 = (u32)lbl_8026FB48;
            r12 = (u32)lbl_8026FB48;
            r20 = 0x0;
            r11 = *(u32*)((u8*)r12 + 0x0);
            r10 = *(u32*)((u8*)r12 + 0x4);
            r9 = *(u32*)((u8*)r12 + 0x8);
            r8 = *(u32*)((u8*)r12 + 0xC);
            r7 = *(u32*)((u8*)r12 + 0x10);
            r6 = *(u32*)((u8*)r12 + 0x14);
            r5 = *(u32*)((u8*)r12 + 0x18);
            r4 = *(u32*)((u8*)r12 + 0x1C);
            r3 = *(u32*)((u8*)r12 + 0x20);
            tmp = *(u32*)((u8*)r12 + 0x24);
            *(u32*)(sp + 0x94) = tmp;
            if (r21 < 0x20) {
                tmp = r26 | (0xc803 << 16);
                if ((s32)r31 != 0) {
                    tmp = r26 | (0xd803 << 16);
                }
                r3 = 0x4E800000;
                *(u32*)(sp + 0x70) = tmp;
                tmp = r3 + 0x20;
                r3 = r23;
                *(u32*)(sp + 0x94) = tmp;
                r4 = 0x28;
                ((void(*)(void))fn_800C0D70)();
                r3 = (u32)lbl_803FED44;
                r12 = (u32)sp + 0x70;
                r4 = (u32)lbl_803FED44;
                r3 = (u32)sp + 0x8;
                ctr_fn = (void(*)(void))r12;
                ctr_fn();
                r20 = 0x0;

            } else if ((s32)r31 == 0) {
                if ((s32)r31 != 0) {
                    r3 = (u32)sp + 0x8;
                    fn_800C11F4();
                } else {

                    r3 = (u32)sp + 0x8;
                    fn_800C1218();
                }
                tmp = 0x0;
                r3 = -0x1;
                tmp = r4 & tmp;
                r3 = r5 & r3;
                *(u32*)(sp + 0x8) = tmp;

            } else {
                if (r21 == 0x21) {
                    if ((s32)r31 == 0) { *(u32*)(sp + 0x8) = tmp; }
                    r3 = (u32)lbl_8026FAF8; r12 = (u32)lbl_8026FAF8;
                    r11 = *(u32*)((u8*)r12 + 0x0); r10 = *(u32*)((u8*)r12 + 0x4);
                    r9 = *(u32*)((u8*)r12 + 0x8); r8 = *(u32*)((u8*)r12 + 0xC);
                    r7 = *(u32*)((u8*)r12 + 0x10); r6 = *(u32*)((u8*)r12 + 0x14);
                    r5 = *(u32*)((u8*)r12 + 0x18); r4 = *(u32*)((u8*)r12 + 0x1C);
                    r3 = *(u32*)((u8*)r12 + 0x20); tmp = *(u32*)((u8*)r12 + 0x24);
                    *(u32*)(sp + 0x44) = tmp;
                    if ((s32)r31 != 0) { r3 = 0x7C9F0000; tmp = 0x90830000; *(u32*)(sp + 0x24) = tmp; }
                    else { r3 = 0x7C9F0000; r4 = 0x80830000; *(u32*)(sp + 0x24) = tmp; }
                    r4 = 0x4E800000; r3 = r22; tmp = r4 + 0x20; r4 = 0x28;
                    *(u32*)(sp + 0x44) = tmp; ((void(*)(void))fn_800C0D70)();
                    r3 = (u32)lbl_803FED44; r12 = (u32)sp + 0x20;
                    r4 = (u32)lbl_803FED44; r3 = (u32)sp + 0x8;
                    ctr_fn = (void(*)(void))r12; ctr_fn();
                    r20 = 0x0;
                    if ((s32)r31 != 0) { r3 = -0x1; tmp = r20 & r20; r3 = r4 & r3; *(u32*)(sp + 0x8) = tmp; }
                }
            }
            r3 = r20;
        }
        r4 = *(u32*)((u8*)r30 + 0x0);
        r26 = r26 + (0x20 << 16);
        r21 = r21 + 0x1;
        tmp = r4 + 0x8;
        *(u32*)((u8*)r30 + 0x0) = tmp;


    }

    tmp = *(u8*)((u8*)r27 + 0xD);
    if (tmp != 0) {
        tmp = 0x0;
        r3 = 0x702;
        *(u32*)((u8*)r30 + 0x0) = tmp;
    }
    r4 = (u32)gTRKExceptionStatus_80313824;
    r7 = (u32)gTRKExceptionStatus_80313824;
    *(u32*)((u8*)r7 + 0x0) = r6;
    *(u32*)((u8*)r7 + 0x4) = r5;
    *(u32*)((u8*)r7 + 0x8) = r4;
    *(u32*)((u8*)r7 + 0xC) = tmp;

    return;
}

/* fn_800C24BC - 0x800C24BC | size: 0xF4 */
void fn_800C24BC(void) {
    extern void fn_800BE844();
    extern void fn_800BEAB4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r6;
    if (r4 > 0x24) {
        r3 = 0x701;
    } else {

        r6 = (u32)gTRKExceptionStatus_80313824;
        r4 = r4 - r3;
        r30 = (u32)gTRKExceptionStatus_80313824;
        r6 = (u32)gTRKCPUState;
        r9 = *(u32*)((u8*)r30 + 0xC);
        r8 = 0x0;
        r12 = r4 + 0x1;
        r11 = *(u32*)((u8*)r30 + 0x0);
        r10 = *(u32*)((u8*)r30 + 0x4);
        r7 = *(u32*)((u8*)r30 + 0x8);
        tmp = r12 << 2;
        *(u8*)((u8*)r30 + 0xD) = r8;
        r4 = r3 << 2;
        r3 = (u32)gTRKCPUState;
        r4 = r3 + r4;
        *(u32*)((u8*)r31 + 0x0) = tmp;
        if ((s32)r7 != 0) {
            r3 = r5;
            r5 = r12;
            fn_800BEAB4();
        } else {

            r3 = r5;
            r5 = r12;
            fn_800BE844();
        }
        tmp = *(u8*)((u8*)r30 + 0xD);
        if (tmp != 0) {
            tmp = 0x0;
            r3 = 0x702;
            *(u32*)((u8*)r31 + 0x0) = tmp;
        }
        r4 = (u32)gTRKExceptionStatus_80313824;
        r7 = (u32)gTRKExceptionStatus_80313824;
        *(u32*)((u8*)r7 + 0x0) = r6;
        *(u32*)((u8*)r7 + 0x4) = r5;
        *(u32*)((u8*)r7 + 0x8) = r4;
        *(u32*)((u8*)r7 + 0xC) = tmp;
    }
    return;
}

/* fn_800C25FC - 0x800C25FC | size: 0x14C */
void fn_800C25FC(void) {
    extern void fn_800C0E60();
    extern void fn_800C0E70();
    extern void fn_800C2748();
    extern void fn_800C3344();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;

    r6 = (u32)gTRKExceptionStatus_80313824;
    tmp = 0x0;
    r31 = (u32)gTRKExceptionStatus_80313824;
    r27 = r4;
    r28 = r5;
    r26 = r3;
    r29 = r7;
    r3 = r27;
    r4 = *(u32*)((u8*)r31 + 0xC);
    r8 = *(u32*)((u8*)r31 + 0x0);
    r6 = *(u32*)((u8*)r31 + 0x4);
    r5 = *(u32*)((u8*)r31 + 0x8);
    *(u8*)((u8*)r31 + 0xD) = tmp;
    fn_800C3344();
    tmp = __cntlzw(r29);
    r4 = *(u32*)((u8*)r28 + 0x0);
    r25 = r3;
    r5 = (u32)tmp >> 5;
    fn_800C2748();
    /* mr. r30, r3 */;
    if ((s32)tmp != 0) {
        tmp = 0x0; *(u32*)((u8*)r28 + 0x0) = tmp;
    } else {
        fn_800C0E60();
        r4 = (u32)gTRKCPUState; r4 = (u32)gTRKCPUState; r8 = r3;
        tmp = *(u32*)((u8*)r4 + 0x1F8); tmp = tmp & 0x00000010; r7 = r8 | tmp;
        if ((s32)r29 != 0) {
            r5 = *(u32*)((u8*)r28 + 0x0); r3 = r26; r4 = r25; r6 = r8; fn_800C0E70();
        } else {
            r5 = *(u32*)((u8*)r28 + 0x0); r3 = r25; r4 = r26; r6 = r7; r7 = r8;
            fn_800C0E70();
            r4 = *(u32*)((u8*)r28 + 0x0); r3 = r25; ((void(*)(void))fn_800C0D70)();
            if (r27 != r25) { r4 = *(u32*)((u8*)r28 + 0x0); r3 = r27; ((void(*)(void))fn_800C0D70)(); }
        }
    }
    tmp = *(u8*)((u8*)r31 + 0xD);
    if (tmp != 0) {
        tmp = 0x0;
        r30 = 0x702;
        *(u32*)((u8*)r28 + 0x0) = tmp;
    }
    r3 = (u32)gTRKExceptionStatus_80313824;
    r7 = (u32)gTRKExceptionStatus_80313824;
    r3 = r30;
    *(u32*)((u8*)r7 + 0x0) = r6;
    *(u32*)((u8*)r7 + 0x4) = r5;
    *(u32*)((u8*)r7 + 0x8) = r4;
    *(u32*)((u8*)r7 + 0xC) = tmp;
    return;
}

/* fn_800C2748 - 0x800C2748 | size: 0x2A4 */
void fn_800C2748(void) {
    extern u8 lbl_8026FAE8[];
    extern void fn_800C2748();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r27 = r4 + r3;
    r26 = r5;
    r5 = 0x700;
    if (r27 < r3) {
        r3 = 0x700;
        return;
    }
    r4 = (u32)lbl_8026FAE8;
    r6 = 0x0;
    r31 = (u32)lbl_8026FAE8;
    tmp = *(u32*)((u8*)r31 + 0x4);
    if (r3 > tmp) { r3 = r5; return; }
    tmp = *(u32*)((u8*)r31 + 0x0);
    if (r27 < tmp) { r3 = r5; return; }
    { s32 pOk = 1;
    if ((s32)r26 == 0) { tmp = r6 << 4; r4 = r31 + tmp; tmp = *(u32*)((u8*)r4 + 0x8); if ((s32)tmp == 0) pOk = 0; }
    if (pOk && (s32)r26 == 1) { r4 = (u32)lbl_8026FAE8; tmp = r6 << 4; r4 = (u32)lbl_8026FAE8; r4 = r4 + tmp; tmp = *(u32*)((u8*)r4 + 0xC); if ((s32)tmp == 0) pOk = 0; }
    if (!pOk) { r5 = 0x700; r3 = r5; return; }
    }
    r4 = (u32)lbl_8026FAE8;
    r29 = r6 << 4;
    r4 = (u32)lbl_8026FAE8;
    r5 = 0x0;
    tmp = *(u32*)(r4 + r29);
    if (r3 < tmp) {
        tmp = tmp - r3;
        r6 = 0x700;
        r30 = tmp + r3;
        if (r30 < r3) { /* overflow */ } else {
            tmp = *(u32*)((u8*)r31 + 0x4); r5 = 0x0;
            if (r3 <= tmp) { tmp = *(u32*)((u8*)r31 + 0x0);
                if (r30 >= tmp) {
                    s32 p2 = 1;
                    if ((s32)r26 == 0) { tmp = r5 << 4; r4 = r4 + tmp; tmp = *(u32*)((u8*)r4 + 0x8); if ((s32)tmp == 0) p2 = 0; }
                    if (p2 && (s32)r26 == 1) { r4 = (u32)lbl_8026FAE8; tmp = r5 << 4; r4 = (u32)lbl_8026FAE8; r4 = r4 + tmp; tmp = *(u32*)((u8*)r4 + 0xC); if ((s32)tmp == 0) p2 = 0; }
                    if (!p2) { r6 = 0x700; } else {
                        r4 = (u32)lbl_8026FAE8; r28 = r5 << 4; r4 = (u32)lbl_8026FAE8; r6 = 0x0;
                        tmp = *(u32*)(r4 + r28);
                        if (r3 < tmp) { r5 = r26; r4 = tmp - r3; fn_800C2748(); r6 = r3; }
                        if ((s32)r6 == 0) {
                            r3 = (u32)lbl_8026FAE8; tmp = (u32)lbl_8026FAE8; r3 = tmp + r28; r3 = *(u32*)((u8*)r3 + 0x4);
                            if (r30 > r3) { r5 = r26; r4 = r30 - r3; fn_800C2748(); r6 = r3; }
                        }
                    }
                }
            }
        }
        r5 = r6;
    }
    if ((s32)r5 != 0) { r3 = r5; return; }
    r3 = (u32)lbl_8026FAE8;
    r4 = (u32)lbl_8026FAE8;
    r28 = r4 + 0x4;
    r3 = *(u32*)(r28 + r29);
    if (r27 <= r3) { r3 = r5; return; }
    tmp = r27 - r3;
    r6 = 0x700;
    r30 = tmp + r3;
    if (r30 < r3) { /* overflow */ } else {
        tmp = *(u32*)((u8*)r31 + 0x4); r5 = 0x0;
        if (r3 <= tmp) { tmp = *(u32*)((u8*)r31 + 0x0);
            if (r30 >= tmp) {
                s32 p3 = 1;
                if ((s32)r26 == 0) { tmp = r5 << 4; r4 = r4 + tmp; tmp = *(u32*)((u8*)r4 + 0x8); if ((s32)tmp == 0) p3 = 0; }
                if (p3 && (s32)r26 == 1) { r4 = (u32)lbl_8026FAE8; tmp = r5 << 4; r4 = (u32)lbl_8026FAE8; r4 = r4 + tmp; tmp = *(u32*)((u8*)r4 + 0xC); if ((s32)tmp == 0) p3 = 0; }
                if (!p3) { r6 = 0x700; } else {
                    r4 = (u32)lbl_8026FAE8; r27 = r5 << 4; r4 = (u32)lbl_8026FAE8; r6 = 0x0;
                    tmp = *(u32*)(r4 + r27);
                    if (r3 < tmp) { r5 = r26; r4 = tmp - r3; fn_800C2748(); r6 = r3; }
                    if ((s32)r6 == 0) {
                        r3 = *(u32*)(r28 + r27);
                        if (r30 > r3) { r5 = r26; r4 = r30 - r3; fn_800C2748(); r6 = r3; }
                    }
                }
            }
        }
    }
    r5 = r6;

    r3 = r5;

    return;
}

/* fn_800C29F0 - 0x800C29F0 | size: 0x8 */
/* TRK support-call trap placeholder */
void fn_800C29F0(void) {
    asm { twi 31, r0, 0 }
}

/* fn_800C29F8 - 0x800C29F8 | size: 0x8 */
/* TRK support-call trap placeholder */
void fn_800C29F8(void) {
    asm { twi 31, r0, 0 }
}

/* fn_800C2A00 - 0x800C2A00 | size: 0x8 */
/* TRK support-call trap placeholder */
void fn_800C2A00(void) {
    asm { twi 31, r0, 0 }
}

/* fn_800C2A08 - 0x800C2A08 | size: 0x8 */
/* TRK support-call trap placeholder */
void fn_800C2A08(void) {
    asm { twi 31, r0, 0 }
}
