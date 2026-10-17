#ifndef TRK_TRK_H
#define TRK_TRK_H

#include "dolphin/types.h"

/*
 * MetroTRK debugger nub types and declarations.
 * Used across all TRK source files.
 */

typedef s32 TRKResult;

/* Event types */
#define kEventNone      0
#define kEventShutdown  1
#define kEventMessage   2
#define kEventBreak     3
#define kEventInterrupt 4
#define kEventSupport   5

/* TRK error codes */
#define kTRKSuccess     0
#define kTRKError       1

/* Number of message buffers */
#define TRK_NUM_BUFFERS 3

/* Buffer size */
#define TRK_BUFFER_SIZE 0x890

/* Event queue size */
#define TRK_EVENT_QUEUE_SIZE 2

/* Event queue entry size */
#define TRK_EVENT_SIZE 0xC

/* TRKEvent structure */
typedef struct TRKEvent {
    s32 type;           /* 0x00: event type */
    s32 unused;         /* 0x04 */
    s32 bufferIndex;    /* 0x08: associated buffer index */
} TRKEvent;

/* TRKBuffer - message buffer header */
typedef struct TRKBuffer {
    s32 mutex;          /* 0x00: mutex/lock */
    s32 inUse;          /* 0x04: usage flag */
    s32 position;       /* 0x08: current position */
    s32 length;         /* 0x0C: data length */
    u8 data[0x880];    /* 0x10: buffer data */
} TRKBuffer;

/* Communication table function pointers */
typedef struct DBCommTable {
    void* initialize;   /* 0x00 */
    void* initInterrupts; /* 0x04 */
    void* shutdown;     /* 0x08 */
    void* peek;         /* 0x0C */
    void* read;         /* 0x10 */
    void* write;        /* 0x14 */
    void* open;         /* 0x18 */
    void* close;        /* 0x1C */
    void* preContinue;  /* 0x20 */
    void* postStop;     /* 0x24 */
} DBCommTable;

/* External globals */
extern u32 gTRKBigEndian;
extern void* gTRKInputPendingPtr;
extern DBCommTable gDBCommTable;
extern u8 TRK_Use_BBA;
extern s32 TRK_mainError;

/* TRKNub.c */
void TRKNubMainLoop(void);
void TRKDestructEvent(TRKEvent* event);
s32  TRKGetNextEvent(TRKEvent* event);
TRKResult TRKInitializeEventQueue(void);
void TRKNubWelcome(void);
TRKResult TRKTerminateNub(void);
TRKResult TRKInitializeNub(void);

/* TRKBuffer.c */
TRKBuffer* TRKGetBuffer(s32 index);
TRKResult TRKInitializeMessageBuffers(void);

/* TRKSerial.c */
TRKResult TRKInitializeSerialHandler(void);
void TRKGetInput(void);

/* TRKDispatch.c */
TRKResult TRKDispatchMessage(TRKBuffer* buffer);
TRKResult TRKInitializeDispatcher(void);

/* TRKInterrupt.c - these are asm */
void TRKInterruptHandler(void);
void TRKExceptionHandler(void);
void TRKSwapAndGo(void);
void TRKInterruptHandlerEnableInterrupts(void);

/* TRKTarget.c */
void TRKTargetSetInputPendingPtr(u8* ptr);
void TRKTargetSetStopped(s32 stopped);
s32  TRKTargetStopped(void);
void TRKTargetSupportRequest(void);
TRKResult TRKTargetInterrupt(TRKEvent* event);
void TRKPostInterruptEvent(void);
TRKResult TRKTargetContinue(void);

/* TRKSaveState.c - asm */
void TRKSaveExtended1Block(void);
void TRKRestoreExtended1Block(void);

/* TRKInit.c */
void InitMetroTRK(u32 debugArg);
void InitMetroTRK_BBA(void);
TRKResult TRKInitializeTarget(void);
void EnableMetroTRKInterrupts(void);
void TRK_main(void);
void TRKLoadContext(void* ctx, u32 exceptionID);

/* TRKBoard.c */
void TRK_board_display(const char* msg);
void UnreserveEXI2Port(void);
void ReserveEXI2Port(void);
void InitializeProgramEndTrap(void);
void TRKUARTInterruptHandler(void);

/* TRKComm.c */
TRKResult TRKInitializeIntDrivenUART(s32 baud, s32 polarity, s32 pad, void* pendingPtr);
s32  InitMetroTRKCommTable(s32 channel);
void TRKEXICallBack(s32 chan, void* ctx);

/* External OS functions used by TRK */
extern void OSReport(const char* fmt, ...);
extern void OSEnableScheduler(void);
extern void ICInvalidateRange(void* addr, u32 size);
extern void DCFlushRange(void* addr, u32 size);
extern void PPCHalt(void);

/* External TRK support functions (asm) */
extern void fn_800C0CC0(void* mutex);  /* TRKReleaseMutex */
extern void fn_800C0CC8(void* mutex);  /* TRKAcquireMutex */
extern void fn_800C0CD0(void* mutex);  /* TRKInitializeMutex */
extern TRKResult TRKDoNotifyStopped(s32 event); /* TRKNubEvent */
extern void fn_800C0E60(void);         /* TRKGetMSR or similar */
extern void fn_80003488(void* dst, const void* src, u32 size); /* memcpy variant */

extern TRKResult TRKTerminateSerialHandler(void);    /* TRKTerminateSerialHandler */
extern TRKResult TRKReleaseBuffer(s32 idx); /* TRKReleaseBuffer */
extern void fn_800BF1FC(void);         /* TRKProcessInput */
extern void TRKSetBufferPosition(void);         /* TRKResetBuffer */
extern void TRKConstructEvent(TRKEvent* event, s32 type);  /* TRKConstructEvent */
extern TRKResult fn_800BE47C(TRKEvent* event);        /* TRKPostEvent */

extern void TRKPollUART(void);         /* comm helper */
extern void EnableEXI2Interrupts(void);         /* EnableEXI2Interrupts */

extern void Hu_IsStub(void);
extern void AMC_IsStub(void);

/* MWTRACE */
extern void MWTRACE(s32 level, const char* fmt, ...);

#endif /* TRK_TRK_H */
