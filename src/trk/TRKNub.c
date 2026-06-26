#include "dolphin/types.h"

/*
 * TRKNub.c - MetroTRK debugger nub main loop and event queue.
 *
 * Implements the core event loop that processes debugger commands,
 * manages the event queue, and coordinates with target continue/stop.
 */

/* Forward declarations for internal TRK functions */
extern void* gTRKInputPendingPtr;
extern void fn_800C0CC0(void* mutex);  /* TRKReleaseMutex */
extern void fn_800C0CC8(void* mutex);  /* TRKAcquireMutex */
extern void fn_800C0CD0(void* mutex);  /* TRKInitializeMutex */
extern void fn_800BEEB4(s32 idx);      /* TRKReleaseBuffer */
extern void fn_800BE464(void* event, s32 type); /* TRKConstructEvent */
extern s32  fn_800BE47C(void* event);  /* TRKPostEvent */
extern void fn_80003488(void* dst, const void* src, u32 size);
extern void fn_800BF080(void);         /* TRKTerminateSerialHandler */
extern s32  fn_800BF1FC(void);         /* TRKProcessInput */
extern void fn_800BEE44(void);         /* TRKResetBuffer */

extern void MWTRACE(s32 level, const char* fmt, ...);
extern void usr_put_initialize(void);

/* Forward declarations for functions defined later in this file */
void TRKDestructEvent(void* event);
s32 TRKGetNextEvent(void* event);

/* Functions in this translation unit or nearby */
extern void* TRKGetBuffer(s32 index);
extern s32 TRKDispatchMessage(void* buffer);
extern void TRKTargetInterrupt(void* event);
extern void TRKTargetSupportRequest(void);
extern s32 TRKTargetStopped(void);
extern s32 TRKTargetContinue(void);
extern void TRKGetInput(void);
extern void TRKTargetSetInputPendingPtr(u8* ptr);
extern s32 TRKInitializeMessageBuffers(void);
extern s32 TRKInitializeDispatcher(void);
extern void InitializeProgramEndTrap(void);
extern s32 TRKInitializeSerialHandler(void);
extern s32 TRKInitializeTarget(void);
extern s32 TRKInitializeIntDrivenUART(s32 baud, s32 polarity, s32 pad, void* pendingPtr);
extern void TRK_board_display(const char* msg);

/* gTRKBigEndian - endianness flag (1=big, 0=little) */
extern u8 gTRKBigEndian[];

/* Event queue structure - 0x28 bytes at lbl_803FCDD8 */
/* Offset 0x00: mutex (4 bytes)
 * Offset 0x04: count (4 bytes)
 * Offset 0x08: head  (4 bytes)
 * Offset 0x0C: events[0] (12 bytes each, 2 entries)
 * Offset 0x24: sequence counter (4 bytes)
 */
extern u8 lbl_803FCDD8[];

/*
 * TRKNubMainLoop - Main debugger event processing loop.
 *
 * Continually dequeues events and dispatches them based on type:
 *   type 1: shutdown
 *   type 2: message - dispatch to command handler
 *   type 3/4: interrupt - send to target interrupt handler
 *   type 5: support request
 *
 * When no events are pending, checks for input or continues target.
 */
void TRKNubMainLoop(void) {
    s32 done = 0;
    s32 firstPass = 0;
    u8 eventBuf[0x10]; /* stack event storage: type at +0, padding at +4, bufIdx at +8 */

    while (done == 0) {
        if (TRKGetNextEvent((void*)eventBuf)) {
            s32 eventType;
            firstPass = 0;

            eventType = *(s32*)&eventBuf[0];
            switch (eventType) {
                case 2: { /* message event */
                    s32 bufIdx = *(s32*)&eventBuf[8];
                    void* buf = TRKGetBuffer(bufIdx);
                    TRKDispatchMessage(buf);
                    break;
                }
                case 1: /* shutdown */
                    done = 1;
                    break;
                case 3: /* break */
                case 4: /* interrupt */
                    TRKTargetInterrupt((void*)eventBuf);
                    break;
                case 5: /* support request */
                    TRKTargetSupportRequest();
                    break;
                default:
                    break;
            }
            TRKDestructEvent((void*)eventBuf);
        } else {
            if (firstPass != 0) {
                u8** ppInput = (u8**)&gTRKInputPendingPtr;
                u8* pInput = *ppInput;
                if (*pInput != 0) {
                    /* Input pending flag was set, try to continue */
                    if (!TRKTargetStopped()) {
                        TRKTargetContinue();
                    }
                    firstPass = 0;
                    continue;
                }
            }
            firstPass = 1;
            TRKGetInput();
        }
    }
}

/*
 * TRKDestructEvent - Release resources associated with an event.
 * Frees the buffer referenced by the event's buffer index field.
 */
void TRKDestructEvent(void* event) {
    s32 bufIdx = ((s32*)event)[2]; /* offset 0x08 */
    fn_800BEEB4(bufIdx);
}

/*
 * TRKGetNextEvent - Dequeue the next event from the event queue.
 * Returns 1 if an event was dequeued, 0 if queue was empty.
 */
s32 TRKGetNextEvent(void* event) {
    s32 result = 0;
    void* queue = (void*)lbl_803FCDD8;

    fn_800C0CC8(queue); /* acquire mutex */

    {
        s32 count = ((s32*)queue)[1]; /* offset 0x04 */
        if (count > 0) {
            s32 head = ((s32*)queue)[2]; /* offset 0x08 */
            void* src = (void*)((u8*)queue + 0x0C + head * 0x0C);
            fn_80003488(event, src, 0x0C);

            head = head + 1;
            count = count - 1;
            ((s32*)queue)[2] = head;
            if (head == 2) {
                ((s32*)queue)[2] = 0;
            }
            ((s32*)queue)[1] = count;
            result = 1;
        }
    }

    fn_800C0CC0(queue); /* release mutex */
    return result;
}

/*
 * TRKInitializeEventQueue - Set up the event queue for use.
 * Creates and acquires the mutex, then zeros the queue state.
 */
s32 TRKInitializeEventQueue(void) {
    void* queue = (void*)lbl_803FCDD8;

    fn_800C0CD0(queue); /* init mutex */
    fn_800C0CC8(queue); /* acquire mutex */

    ((s32*)queue)[1] = 0;   /* count = 0 */
    ((s32*)queue)[2] = 0;   /* head = 0 */
    ((s32*)queue)[9] = 0x100; /* sequence = 0x100 */

    fn_800C0CC0(queue); /* release mutex */

    return 0;
}

/*
 * TRKNubWelcome - Display the TRK welcome banner.
 */
void TRKNubWelcome(void) {
    TRK_board_display("MetroTRK for Dolphin different from R4.0");
}

/*
 * TRKTerminateNub - Shut down the debugger nub.
 * Calls the serial handler terminator.
 */
s32 TRKTerminateNub(void) {
    fn_800BF080();
    return 0;
}

/*
 * TRKInitializeNub - Initialize all TRK subsystems.
 *
 * Performs endian detection, then initializes the event queue,
 * message buffers, dispatcher, serial handler, target, and UART.
 * Returns 0 on success, nonzero on failure.
 */
s32 TRKInitializeNub(void) {
    s32 err = 0;
    s32 uartErr;
    u8 testBytes[4];

    /* Endian detection pattern: 0x12345678 */
    testBytes[0] = 0x12;
    testBytes[1] = 0x34;
    testBytes[2] = 0x56;
    testBytes[3] = 0x78;

    {
        u32 testWord = *(u32*)testBytes;
        u32* pBigEndian;

        /* gTRKBigEndian declared at file scope as u8[] */
        pBigEndian = (u32*)gTRKBigEndian;
        *pBigEndian = 1; /* assume big endian initially */

        if (testWord == 0x12345678) {
            *pBigEndian = 1; /* big endian confirmed */
        } else if (testWord == 0x78563412) {
            *pBigEndian = 0; /* little endian */
        } else {
            err = 1; /* unknown endianness */
        }
    }

    MWTRACE(1, "TRKInitializeNub - starting\n");

    if (err == 0) {
        usr_put_initialize();
    }

    if (err == 0) {
        err = TRKInitializeEventQueue();
    }

    if (err == 0) {
        err = TRKInitializeMessageBuffers();
    }

    if (err == 0) {
        err = TRKInitializeDispatcher();
    }

    InitializeProgramEndTrap();

    if (err == 0) {
        err = TRKInitializeSerialHandler();
    }

    if (err == 0) {
        err = TRKInitializeTarget();
    }

    if (err == 0) {
        u8** ppInput;
        u8* inputPtr;

        extern void* gTRKInputPendingPtr;
        ppInput = (u8**)&gTRKInputPendingPtr;

        uartErr = TRKInitializeIntDrivenUART(0xE100, 1, 0, (void*)ppInput);

        inputPtr = *ppInput;
        TRKTargetSetInputPendingPtr(inputPtr);

        if (uartErr != 0) {
            err = uartErr;
        }
    }

    return err;
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* MessageSend - 0x800BE800 | size: 0x44 */
s32 MessageSend(u8* p) {
    extern char lbl_8026F640[];
    extern s32 fn_800C3588(void* dst, u32 val);

    s32 r = fn_800C3588(p + 0x10, *(u32*)(p + 0x8));
    MWTRACE(1, lbl_8026F640, r);
    return 0;
}

/* fn_800BE844 - 0x800BE844 | size: 0xF0 */
void fn_800BE844(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r6 = (u32)gTRKBigEndian;
    r28 = r3;
    r29 = r5;
    r31 = r4;
    r27 = (u32)gTRKBigEndian;
    r30 = 0x0;
    r3 = 0x0;
    while ((s32)r3 == 0 && (s32)r30 < (s32)r29) {
        tmp = *(u32*)((u8*)r27 + 0x0);
        if ((s32)tmp != 0) {
            r25 = r31;
        } else {
            r25 = (u32)sp + 0x8;
        }
        r3 = *(u32*)((u8*)r28 + 0xC);
        r24 = 0x4;
        tmp = *(u32*)((u8*)r28 + 0x8);
        r26 = 0x0;
        tmp = tmp - r3;
        if (r24 > tmp) {
            r26 = 0x302;
            r24 = tmp;
        }
        r4 = r3 + 0x10;
        r3 = r25;
        r5 = r24;
        r4 = r28 + r4;
        ((void(*)(void))fn_80003488)();
        tmp = *(u32*)((u8*)r28 + 0xC);
        tmp = tmp + r24;
        *(u32*)((u8*)r28 + 0xC) = tmp;
        tmp = *(u32*)((u8*)r27 + 0x0);
        if (((s32)tmp == 0) && ((s32)r26 == 0)) {
            tmp = *(u8*)((u8*)r25 + 0x3);
            *(u8*)((u8*)r31 + 0x0) = tmp;
            tmp = *(u8*)((u8*)r25 + 0x2);
            *(u8*)((u8*)r31 + 0x1) = tmp;
            tmp = *(u8*)((u8*)r25 + 0x1);
            *(u8*)((u8*)r31 + 0x2) = tmp;
            tmp = *(u8*)((u8*)r25 + 0x0);
            *(u8*)((u8*)r31 + 0x3) = tmp;
        }
        r3 = r26;
        r31 = r31 + 0x4;
        r30 = r30 + 0x1;
    }

    return;
}

/* fn_800BE934 - 0x800BE934 | size: 0x98 */
void fn_800BE934(void) {
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r26 = r3;
    r27 = r4;
    r28 = r5;
    r29 = 0x0;
    r3 = 0x0;
    while ((s32)r3 == 0 && (s32)r29 < (s32)r28) {
        r3 = *(u32*)((u8*)r26 + 0xC);
        r30 = 0x1;
        tmp = *(u32*)((u8*)r26 + 0x8);
        r31 = 0x0;
        tmp = tmp - r3;
        if (r30 > tmp) {
            r31 = 0x302;
            r30 = tmp;
        }
        r4 = r3 + 0x10;
        r5 = r30;
        r3 = r27 + r29;
        r4 = r26 + r4;
        ((void(*)(void))fn_80003488)();
        tmp = *(u32*)((u8*)r26 + 0xC);
        r3 = r31;
        r29 = r29 + 0x1;
        tmp = tmp + r30;
        *(u32*)((u8*)r26 + 0xC) = tmp;
    }

    return;
}

/* fn_800BE9CC - 0x800BE9CC | size: 0xE8 */
void fn_800BE9CC(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = (u32)gTRKBigEndian;
    r27 = r3;
    r30 = r4;
    tmp = *(u32*)gTRKBigEndian;
    if ((s32)tmp != 0) {
        r31 = r30;
    } else {

        r31 = (u32)sp + 0x8;
    }
    r3 = *(u32*)((u8*)r27 + 0xC);
    r28 = 0x8;
    tmp = *(u32*)((u8*)r27 + 0x8);
    r29 = 0x0;
    tmp = tmp - r3;
    if (r28 > tmp) {
        r29 = 0x302;
        r28 = tmp;
    }
    r4 = r3 + 0x10;
    r3 = r31;
    r5 = r28;
    r4 = r27 + r4;
    ((void(*)(void))fn_80003488)();
    tmp = *(u32*)((u8*)r27 + 0xC);
    r3 = (u32)gTRKBigEndian;
    tmp = tmp + r28;
    *(u32*)((u8*)r27 + 0xC) = tmp;
    tmp = *(u32*)gTRKBigEndian;
    if (((s32)tmp == 0) && ((s32)r29 == 0)) {

        tmp = *(u8*)((u8*)r31 + 0x7);
        *(u8*)((u8*)r30 + 0x0) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x6);
        *(u8*)((u8*)r30 + 0x1) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x5);
        *(u8*)((u8*)r30 + 0x2) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x4);
        *(u8*)((u8*)r30 + 0x3) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x3);
        *(u8*)((u8*)r30 + 0x4) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x2);
        *(u8*)((u8*)r30 + 0x5) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x1);
        *(u8*)((u8*)r30 + 0x6) = tmp;
        tmp = *(u8*)((u8*)r31 + 0x0);
        *(u8*)((u8*)r30 + 0x7) = tmp;
    }
    r3 = r29;
    return;
}

/* fn_800BEAB4 - 0x800BEAB4 | size: 0xFC */
void fn_800BEAB4(void) {
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r6 = (u32)gTRKBigEndian;
    r27 = r3;
    r28 = r5;
    r30 = r4;
    r31 = (u32)gTRKBigEndian;
    r29 = 0x0;
    r3 = 0x0;
    while ((s32)r3 == 0 && (s32)r29 < (s32)r28) {
        tmp = *(u32*)((u8*)r31 + 0x0);
        r3 = *(u32*)((u8*)r30 + 0x0);
        if ((s32)tmp != 0) {
            r4 = (u32)sp + 0x8;
        } else {
            r6 = *(u8*)(sp + 0xB);
            r4 = (u32)sp + 0xc;
            r5 = *(u8*)(sp + 0xA);
            r3 = *(u8*)(sp + 0x9);
            tmp = *(u8*)(sp + 0x8);
            *(u8*)(sp + 0xC) = r6;
            *(u8*)(sp + 0xD) = r5;
            *(u8*)(sp + 0xE) = r3;
            *(u8*)(sp + 0xF) = tmp;
        }
        r5 = *(u32*)((u8*)r27 + 0xC);
        r25 = 0x4;
        r26 = 0x0;
        tmp = 0x880 - r5;
        if (tmp < 4) {
            r26 = 0x301;
            r25 = tmp;
        }
        if (r25 == 1) {
            r3 = *(u8*)((u8*)r4 + 0x0);
            tmp = r5 + 0x10;
            *(u8*)(r27 + tmp) = r3;
        } else {
            r3 = r5 + 0x10;
            r5 = r25;
            r3 = r27 + r3;
            ((void(*)(void))fn_80003488)();
        }
        tmp = *(u32*)((u8*)r27 + 0xC);
        r3 = r26;
        r30 = r30 + 0x4;
        r29 = r29 + 0x1;
        tmp = tmp + r25;
        *(u32*)((u8*)r27 + 0xC) = tmp;
        tmp = *(u32*)((u8*)r27 + 0xC);
        *(u32*)((u8*)r27 + 0x8) = tmp;
    }

    return;
}

/* fn_800BEBB0 - 0x800BEBB0 | size: 0x68 */
s32 fn_800BEBB0(u8* buf, u8* src, s32 count) {
    u8 b;
    s32 i = 0;
    s32 err = 0;

    while (err == 0 && i < count) {
        u32 r = *(u32*)(buf + 0xC);
        b = *src;
        if (r >= 0x880) {
            r = 0x301;
        } else {
            *(u32*)(buf + 0xC) = r + 1;
            *(buf + 0x10 + r) = b;
            r = 0x0;
            *(u32*)(buf + 0x8) = *(u32*)(buf + 0x8) + 1;
        }
        err = r;
        i = i + 1;
        src = src + 1;
    }

    return err;
}

/* fn_800BEC18 - 0x800BEC18 | size: 0xFC */
s32 fn_800BEC18(u8* buf, u32 unused, u32 w0, u32 w1) {
    u8 swap[8];
    u8 raw[8];
    u8* src;
    u32 writepos;
    s32 err;
    u32 n;

    *(u32*)&raw[0] = w0;
    *(u32*)&raw[4] = w1;

    if (*(s32*)gTRKBigEndian != 0) {
        src = raw;
    } else {
        swap[0] = raw[7];
        swap[1] = raw[6];
        swap[2] = raw[5];
        swap[3] = raw[4];
        swap[4] = raw[3];
        swap[5] = raw[2];
        swap[6] = raw[1];
        swap[7] = raw[0];
        src = swap;
    }

    writepos = *(u32*)(buf + 0xC);
    n = 8;
    err = 0;
    if (0x880 - writepos < 8) {
        err = 0x301;
        n = 0x880 - writepos;
    }
    if (n == 1) {
        buf[writepos + 0x10] = *src;
    } else {
        fn_80003488(buf + writepos + 0x10, src, n);
    }
    *(u32*)(buf + 0xC) = *(u32*)(buf + 0xC) + n;
    *(u32*)(buf + 0x8) = *(u32*)(buf + 0xC);
    return err;
}

/* fn_800BED14 - 0x800BED14 | size: 0x8C */
s32 fn_800BED14(u8* buf, u8* dst, u32 n) {
    s32 err = 0;
    if (n == 0) {
        return 0;
    }
    {
        u32 writepos = *(u32*)(buf + 0xC);
        u32 endpos = *(u32*)(buf + 0x8);
        u32 space = endpos - writepos;
        if (n > space) {
            err = 0x302;
            n = space;
        }
        fn_80003488(dst, buf + writepos + 0x10, n);
        *(u32*)(buf + 0xC) = *(u32*)(buf + 0xC) + n;
    }
    return err;
}

/* fn_800BEDA0 - 0x800BEDA0 | size: 0xA4 */
s32 fn_800BEDA0(u8* buf, u8* src, u32 n) {
    s32 err = 0;
    if (n == 0) {
        return 0;
    }
    {
        u32 writepos = *(u32*)(buf + 0xC);
        u32 space = 0x880 - writepos;
        if (space < n) {
            err = 0x301;
            n = space;
        }
        if (n == 1) {
            buf[writepos + 0x10] = src[0];
        } else {
            fn_80003488(buf + writepos + 0x10, src, n);
        }
        *(u32*)(buf + 0xC) = *(u32*)(buf + 0xC) + n;
        *(u32*)(buf + 0x8) = *(u32*)(buf + 0xC);
    }
    return err;
}

/* fn_800BEE74 - 0x800BEE74 | size: 0x40
 * TRKResetBuffer - Clear a TRK buffer's read/write positions
 * and optionally zero the data area (0x880 bytes at offset 0x10).
 */
void fn_800BEE74(u8* buf, s32 keepData) {
    extern void fn_80003458(void* dst, s32 val, u32 len);

    *(u32*)(buf + 0x8) = 0;
    *(u32*)(buf + 0xC) = 0;
    if (keepData == 0) {
        fn_80003458(buf + 0x10, 0, 0x880);
    }
}

