#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSTime.h"

#define OS_BUS_CLOCK (*(u32*) 0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)
#define OSMicrosecondsToTicks(usec) \
    (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)

extern volatile u32 __DIRegs[16] : 0xCC006000;
extern volatile u32 __PIRegs[12] : 0xCC003000;

extern volatile BOOL StopAtNextInt;
extern DVDLowCallback Callback;
extern volatile OSTime LastResetEnd;
extern volatile u32 ResetOccurred;
extern volatile BOOL WaitingCoverClose;
extern OSAlarm AlarmForTimeout;
extern u32 lbl_8047A784;
extern DVDLowCallback lbl_8047A78C;
extern volatile BOOL lbl_8047A7A0;
extern volatile u32 WorkAroundType;
extern u32 WorkAroundSeekLocation;
extern volatile OSTime lbl_8047A7B0;
extern OSTime lbl_8047A7B8;
extern volatile BOOL lbl_8047A7C0;
extern volatile u32 NextCommandNumber;
extern BOOL lbl_804789B8;

extern void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context);
extern u32 __OSMaskInterrupts(u32 mask);
extern void OSClearContext(OSContext* context);
extern void OSSetCurrentContext(OSContext* context);
extern DVDDiskID* fn_800A7BCC(void);

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

extern DVDLowCommand CommandList[];

#define DVD_ALARM_FOR_WA ((OSAlarm*)((u8*)CommandList + 0x40))
#define DVD_PREV_BUFFER ((DVDLowBuffer*)((u8*)CommandList + 0xBC))
#define DVD_CURR_BUFFER ((DVDLowBuffer*)((u8*)CommandList + 0xC4))

static void Read(void* address, u32 length, u32 offset,
                 DVDLowCallback callback);

void fn_800A41D0(OSAlarm* alarm, OSContext* context)
{
    s32 command = NextCommandNumber;

    (void)alarm;
    (void)context;
    if (CommandList[command].command == 1) {
        ++NextCommandNumber;
        Read(CommandList[command].address, CommandList[command].length,
             CommandList[command].offset, CommandList[command].callback);
    } else if (CommandList[command].command == 2) {
        ++NextCommandNumber;
        DVDLowSeek(CommandList[command].offset,
                   CommandList[command].callback);
    }
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
    Callback = callback;
    StopAtNextInt = FALSE;
    lbl_8047A7C0 = TRUE;
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

static void SeekTwiceBeforeRead(void* address, u32 length, u32 offset,
                                DVDLowCallback callback)
{
    DVDLowCommand* commands = CommandList;
    u32 newOffset;

    if ((offset & ~0x7FFF) == 0) {
        newOffset = 0;
    } else {
        newOffset = (offset & ~0x7FFF) + WorkAroundSeekLocation;
    }

    commands[0].command = 2;
    commands[0].offset = newOffset;
    commands[0].callback = callback;
    commands[1].command = 1;
    commands[1].address = address;
    commands[1].length = length;
    commands[1].offset = offset;
    commands[1].callback = callback;
    commands[2].command = -1;
    NextCommandNumber = 0;
    DVDLowSeek(newOffset, callback);
}

BOOL DVDLowRead(void* address, u32 length, u32 offset,
                DVDLowCallback callback)
{
    u32 previousEnd;
    u32 currentStart;
    OSTime difference;
    DVDLowBuffer* previous = DVD_PREV_BUFFER;
    DVDLowBuffer* current = DVD_CURR_BUFFER;

    __DIRegs[6] = length;
    current->address = address;
    current->length = length;
    current->offset = offset;

    if (WorkAroundType == 0) {
        CommandList[0].command = -1;
        NextCommandNumber = 0;
        Read(address, length, offset, callback);
    } else if (WorkAroundType == 1) {
        if (lbl_804789B8) {
            SeekTwiceBeforeRead(address, length, offset, callback);
        } else {
            u32 previousEndBlock =
                (previous->offset + previous->length - 1) >> 15;
            u32 currentStartBlock = current->offset >> 15;
            u32 cacheBlocks = fn_800A7BCC()->streaming ? 5 : 15;
            BOOL hitCache =
                currentStartBlock > previousEndBlock - 2 ||
                currentStartBlock <
                    previousEndBlock + cacheBlocks + 3;

            if (!hitCache) {
                CommandList[0].command = -1;
                NextCommandNumber = 0;
                Read(address, length, offset, callback);
            } else {
                previousEnd =
                    (previous->offset + previous->length - 1) >> 15;
                currentStart = current->offset >> 15;
                if (previousEnd == currentStart ||
                    previousEnd + 1 == currentStart)
                {
                    difference = __OSGetSystemTime() - lbl_8047A7B0;
                    if (OS_BUS_CLOCK / 4 / 1000 * 5 < difference) {
                        CommandList[0].command = -1;
                        NextCommandNumber = 0;
                        Read(address, length, offset, callback);
                    } else {
                        CommandList[0].command = 1;
                        CommandList[0].address = address;
                        CommandList[0].length = length;
                        CommandList[0].offset = offset;
                        CommandList[0].callback = callback;
                        CommandList[1].command = -1;
                        NextCommandNumber = 0;
                        OSCreateAlarm(DVD_ALARM_FOR_WA);
                        OSSetAlarm(
                            DVD_ALARM_FOR_WA,
                            OS_BUS_CLOCK / 4 / 1000 * 5 - difference +
                                OSMicrosecondsToTicks(500),
                            fn_800A41D0);
                    }
                } else {
                    SeekTwiceBeforeRead(address, length, offset, callback);
                }
            }
        }
    }
    return TRUE;
}

BOOL fn_800A48DC(DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xE0000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL fn_800A4C80(void)
{
    StopAtNextInt = TRUE;
    lbl_8047A7A0 = TRUE;
    return TRUE;
}

DVDLowCallback fn_800A4C94(void)
{
    DVDLowCallback old;

    __DIRegs[1] = 0;
    old = Callback;
    Callback = NULL;
    return old;
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
    OSTime duration;
    OSTime resetStart;

    __DIRegs[1] = 2;
    reg = __PIRegs[9];
    __PIRegs[9] = (reg & ~4) | 1;

    duration = OSMicrosecondsToTicks(12);
    resetStart = __OSGetSystemTime();
    while ((__OSGetSystemTime() - resetStart) < duration) {
    }

    __PIRegs[9] = reg | 5;
    ResetOccurred = TRUE;
    LastResetEnd = __OSGetSystemTime();
}
