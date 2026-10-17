#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSClock.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"

#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)

extern volatile u32 __DIRegs[16] : 0xCC006000;
extern volatile u32 __PIRegs[12] : 0xCC003000;

extern void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context);
extern u32 __OSMaskInterrupts(u32 mask);
extern void OSClearContext(OSContext* context);
extern void OSSetCurrentContext(OSContext* context);
extern DVDDiskID* DVDGetCurrentDiskID(void);

typedef struct DVDLowCommand {
    s32 command;
    void* address;
    u32 length;
    u32 offset;
    DVDLowCallback callback;
} DVDLowCommand;

typedef struct DVDLowBuffer {
    void* address;
    u32 length;
    u32 offset;
} DVDLowBuffer;

static BOOL lbl_804789B8 = TRUE;
static volatile BOOL StopAtNextInt = FALSE;
static u32 lbl_8047A784 = 0;
static DVDLowCallback Callback = NULL;
static DVDLowCallback lbl_8047A78C = NULL;
static volatile OSTime LastResetEnd = 0;
static volatile u32 ResetOccurred = FALSE;
static volatile BOOL WaitingCoverClose = FALSE;
static volatile BOOL lbl_8047A7A0 = FALSE;
static volatile u32 WorkAroundType = 0;
static u32 WorkAroundSeekLocation = 0;
static volatile OSTime lbl_8047A7B0 = 0;
static OSTime lbl_8047A7B8 = 0;
static volatile BOOL lbl_8047A7C0 = FALSE;
static volatile u32 NextCommandNumber = 0;

static DVDLowCommand CommandList[3];
static OSAlarm AlarmForWA;
static OSAlarm AlarmForTimeout;
static OSAlarm AlarmForBreak;
static DVDLowBuffer Prev;
static DVDLowBuffer Curr;

void __DVDInitWA(void)
{
    NextCommandNumber = 0;
    CommandList[0].command = -1;
    __DVDLowSetWAType(0, 0);
    OSInitAlarm();
}

static void Read(void* address, u32 length, u32 offset,
                 DVDLowCallback callback);

static inline BOOL ProcessNextCommand(void)
{
    s32 n = NextCommandNumber;

    if (CommandList[n].command == 1) {
        ++NextCommandNumber;
        Read(CommandList[n].address, CommandList[n].length,
             CommandList[n].offset, CommandList[n].callback);
        return TRUE;
    } else if (CommandList[n].command == 2) {
        ++NextCommandNumber;
        DVDLowSeek(CommandList[n].offset, CommandList[n].callback);
        return TRUE;
    }

    return FALSE;
}

void __DVDInterruptHandler(__OSInterrupt interrupt, OSContext* context)
{
    DVDLowCallback cb;
    OSContext exceptionContext;
    u32 cause = 0;
    u32 reg;
    u32 intr;
    u32 mask;

    if (lbl_8047A7C0) {
        lbl_8047A7B0 = __OSGetSystemTime();
        lbl_804789B8 = FALSE;
        Prev.address = Curr.address;
        Prev.length = Curr.length;
        Prev.offset = Curr.offset;
        if (StopAtNextInt == TRUE) {
            cause |= 8;
        }
    }

    lbl_8047A7C0 = FALSE;
    StopAtNextInt = FALSE;
    reg = __DIRegs[0];
    mask = reg & 0x2A;
    intr = (reg & 0x54) & (mask << 1);

    if (intr & 0x40) {
        cause |= 8;
    }

    if (intr & 0x10) {
        cause |= 1;
    }

    if (intr & 4) {
        cause |= 2;
    }

    if (cause) {
        ResetOccurred = FALSE;
        OSCancelAlarm(&AlarmForTimeout);
    }

    __DIRegs[0] = intr | mask;

    if (ResetOccurred &&
        (__OSGetSystemTime() - LastResetEnd) < OSMillisecondsToTicks(200)) {
        reg = __DIRegs[1];
        mask = reg & 2;
        intr = (reg & 4) & (mask << 1);
        if (intr & 4) {
            if (lbl_8047A78C) {
                lbl_8047A78C(4);
            }
            lbl_8047A78C = NULL;
        }

        __DIRegs[1] = __DIRegs[1];
    } else if (WaitingCoverClose) {
        reg = __DIRegs[1];
        mask = reg & 2;
        intr = (reg & 4) & (mask << 1);

        if (intr & 4) {
            cause |= 4;
        }

        __DIRegs[1] = intr | mask;
        WaitingCoverClose = FALSE;
    } else {
        __DIRegs[1] = 0;
    }

    if ((cause & 8) && !lbl_8047A7A0) {
        cause &= ~8;
    }

    if ((cause & 1)) {
        if (ProcessNextCommand()) {
            return;
        }
    } else {
        CommandList[0].command = -1;
        NextCommandNumber = 0;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    if (cause) {
        cb = Callback;
        Callback = NULL;
        if (cb) {
            cb(cause);
        }

        lbl_8047A7A0 = FALSE;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void fn_800A41D0(OSAlarm* alarm, OSContext* context)
{
    BOOL error = ProcessNextCommand();
}

void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context)
{
    DVDLowCallback callback;
    OSContext exceptionContext;

    (void)alarm;
    __OSMaskInterrupts(0x400);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    callback = Callback;
    Callback = NULL;
    if (callback != NULL) {
        callback(0x10);
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static inline void SetTimeoutAlarm(OSTime timeout)
{
    OSCreateAlarm(&AlarmForTimeout);
    OSSetAlarm(&AlarmForTimeout, timeout, AlarmHandlerForTimeout);
}

static void Read(void* address, u32 length, u32 offset,
                 DVDLowCallback callback)
{

    StopAtNextInt = FALSE;
    lbl_8047A7C0 = TRUE;
    Callback = callback;
    lbl_8047A7B8 = __OSGetSystemTime();

    __DIRegs[2] = 0xA8000000;
    __DIRegs[3] = offset / 4;
    __DIRegs[4] = length;
    __DIRegs[5] = (u32)address;
    __DIRegs[6] = length;
    lbl_8047A784 = length;
    __DIRegs[7] = 3;

    if (length > 0xA00000) {
        SetTimeoutAlarm(OSSecondsToTicks(20));
    } else {
        SetTimeoutAlarm(OSSecondsToTicks(10));
    }
}

static inline BOOL HitCache(DVDLowBuffer* cur, DVDLowBuffer* prev)
{
    u32 prevBlock = (prev->offset + prev->length - 1) >> 15;
    u32 curBlock = cur->offset >> 15;
    u32 cacheBlocks = (DVDGetCurrentDiskID()->streaming ? TRUE : FALSE) ? 5 : 15;

    if ((curBlock > prevBlock - 2) || (curBlock < prevBlock + cacheBlocks + 3)) {
        return TRUE;
    }
    return FALSE;
}

static inline void DoJustRead(void* address, u32 length,
                              u32 offset, DVDLowCallback callback)
{
    CommandList[0].command = -1;
    NextCommandNumber = 0;
    Read(address, length, offset, callback);
}

static void SeekTwiceBeforeRead(void* address, u32 length, u32 offset,
                                DVDLowCallback callback)
{
    u32 newOffset;

    if ((offset & ~0x7FFF) == 0) {
        newOffset = 0;
    } else {
        newOffset = (offset & ~0x7FFF) + WorkAroundSeekLocation;
    }

    CommandList[0].command = 2;
    CommandList[0].offset = newOffset;
    CommandList[0].callback = callback;
    CommandList[1].command = 1;
    CommandList[1].address = address;
    CommandList[1].length = length;
    CommandList[1].offset = offset;
    CommandList[1].callback = callback;
    CommandList[2].command = -1;
    NextCommandNumber = 0;
    DVDLowSeek(newOffset, callback);
}

static inline void WaitBeforeRead(void* address, u32 length,
                                  u32 offset, DVDLowCallback callback,
                                  OSTime timeout)
{
    CommandList[0].command = 1;
    CommandList[0].address = address;
    CommandList[0].length = length;
    CommandList[0].offset = offset;
    CommandList[0].callback = callback;
    CommandList[1].command = -1;
    NextCommandNumber = 0;
    OSCreateAlarm(&AlarmForWA);
    OSSetAlarm(&AlarmForWA, timeout, fn_800A41D0);
}

BOOL DVDLowRead(void* address, u32 length, u32 offset,
                DVDLowCallback callback)
{
    OSTime diff;
    u32 prev;

    __DIRegs[6] = length;
    Curr.address = address;
    Curr.length = length;
    Curr.offset = offset;

    if (WorkAroundType == 0) {
        DoJustRead(address, length, offset, callback);
    } else if (WorkAroundType == 1) {
        if (lbl_804789B8) {
            SeekTwiceBeforeRead(address, length, offset, callback);
        } else {
            if (!HitCache(&Curr, &Prev)) {
                DoJustRead(address, length, offset, callback);
            } else {
                prev = (Prev.offset + Prev.length - 1) >> 15;
                if (prev == Curr.offset >> 15 ||
                    prev + 1 == Curr.offset >> 15)
                {
                    diff = __OSGetSystemTime() - lbl_8047A7B0;
                    if (OSMillisecondsToTicks(5) < diff) {
                        DoJustRead(address, length, offset, callback);
                    } else {
                        WaitBeforeRead(address, length, offset, callback,
                                       OSMillisecondsToTicks(5) - diff +
                                           OSMicrosecondsToTicks(500));
                    }
                } else {
                    SeekTwiceBeforeRead(address, length, offset, callback);
                }
            }
        }
    }
    return TRUE;
}

BOOL DVDLowSeek(u32 offset, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xAB000000;
    __DIRegs[3] = offset / 4;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowWaitCoverClose(DVDLowCallback callback)
{
    Callback = callback;
    WaitingCoverClose = TRUE;
    StopAtNextInt = FALSE;
    __DIRegs[1] = 2;
    return TRUE;
}

BOOL DVDLowReadDiskID(DVDDiskID* diskID, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xA8000040;
    __DIRegs[3] = 0;
    __DIRegs[4] = sizeof(DVDDiskID);
    __DIRegs[5] = (u32) diskID;
    __DIRegs[6] = sizeof(DVDDiskID);
    __DIRegs[7] = 3;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowStopMotor(DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xE3000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowRequestError(DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xE0000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowInquiry(DVDDriveInfo* info, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0x12000000;
    __DIRegs[4] = sizeof(DVDDriveInfo);
    __DIRegs[5] = (u32) info;
    __DIRegs[6] = sizeof(DVDDriveInfo);
    __DIRegs[7] = 3;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowAudioStream(u32 subcmd, u32 length, u32 offset,
                       DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = subcmd | 0xE1000000;
    __DIRegs[3] = offset >> 2;
    __DIRegs[4] = length;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowRequestAudioStatus(u32 subcmd, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = subcmd | 0xE2000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowAudioBufferConfig(BOOL enable, u32 size,
                             DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xE4000000 | (enable != 0 ? 0x10000 : 0) | size;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

void DVDLowReset(void)
{
    u32 reg;
    OSTime resetStart;

    __DIRegs[1] = 2;
    reg = __PIRegs[9];
    __PIRegs[9] = (reg & ~4) | 1;

    resetStart = __OSGetSystemTime();
    while ((__OSGetSystemTime() - resetStart) < OSMicrosecondsToTicks(12)) {
    }

    __PIRegs[9] = reg | 5;
    ResetOccurred = TRUE;
    LastResetEnd = __OSGetSystemTime();
}

BOOL DVDLowBreak(void)
{
    StopAtNextInt = TRUE;
    lbl_8047A7A0 = TRUE;
    return TRUE;
}

DVDLowCallback DVDLowClearCallback(void)
{
    DVDLowCallback old;

    __DIRegs[1] = 0;
    old = Callback;
    Callback = NULL;
    return old;
}

void __DVDLowSetWAType(u32 type, u32 location)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    WorkAroundType = type;
    WorkAroundSeekLocation = location;
    OSRestoreInterrupts(enabled);
}
