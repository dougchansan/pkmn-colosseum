#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSTime.h"

/*
 * DVDLow.c - Low-level DVD drive interface.
 *
 * Handles direct communication with the DVD controller hardware
 * at 0xCC006000. Implements workaround logic and timeout handling.
 *
 * Matches: 0x800A3EB0 - 0x800A4CF0
 */

/* DVD hardware registers at 0xCC006000 */
#define DVD_REG_BASE    ((volatile u32*)0xCC006000)
#define DVD_STATUS      (DVD_REG_BASE[0])
#define DVD_COVER       (DVD_REG_BASE[1])
#define DVD_CMD         (DVD_REG_BASE[2])
#define DVD_OFFSET_LO   (DVD_REG_BASE[3])
#define DVD_LENGTH      (DVD_REG_BASE[4])
#define DVD_DMA_ADDR    (DVD_REG_BASE[5])
#define DVD_DMA_LEN     (DVD_REG_BASE[6])
#define DVD_CONTROL     (DVD_REG_BASE[7])

/* DI reset register at 0xCC003024 */
#define DI_RESET_REG    (*(volatile u32*)0xCC003024)

/* Boot info at 0x80000000 */
#define BUS_CLOCK       (*(u32*)0x800000F8)

/* Workaround command list structure */
typedef struct {
    u32 type;       /* 0x00: command type (1=read, 2=seek, -1=none) */
    u32 cmd;        /* 0x04 */
    u32 addr;       /* 0x08 */
    u32 offset;     /* 0x0C */
    u32 callback;   /* 0x10 */
} WACommand;

/* SDA-relative globals - names match assembly symbol table */
static u32 StopAtNextInt_8047A780;
static u32 WorkAroundSeekLocation_8047A7A8;
static DVDCBCallback Callback_8047A788;
static u32 WaitingCoverClose_8047A79C;
static u32 ResetOccurred_8047A798;
static u32 NextCommandNumber_8047A7C4;
static u32 WorkAroundType_8047A7A4;
static s64 LastResetEnd_8047A790;

/* Forward declarations */
static void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context);
extern void OSInitAlarm(void);
extern s64 __OSGetSystemTime(void);

/* BSS globals */
extern WACommand CommandList_803FC290[];
extern OSAlarm AlarmForTimeout_803FC2F8;

/*
 * __DVDInitWA - Initialize workaround system
 * 0x800A3EB0 | size: 0x40
 */
void __DVDInitWA(void) {
    NextCommandNumber_8047A7C4 = 0;
    CommandList_803FC290[0].type = (u32)-1;
    __DVDLowSetWAType(0, 0);
    OSInitAlarm();
}

/*
 * AlarmHandlerForTimeout - Called when a DVD command times out
 * 0x800A4254 | size: 0x70
 */
static void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context) {
    OSContext exceptionContext;
    DVDCBCallback cb;

    __OSMaskInterrupts(0x00000400);

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    cb = Callback_8047A788;
    Callback_8047A788 = NULL;
    if (cb != NULL) {
        ((void (*)(s32))cb)(0x10);
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

/*
 * DVDLowWaitCoverClose - Wait for the DVD cover to be closed
 * 0x800A4780 | size: 0x2C
 */
BOOL DVDLowWaitCoverClose(DVDCBCallback callback) {
    Callback_8047A788 = callback;
    WaitingCoverClose_8047A79C = 1;
    StopAtNextInt_8047A780 = 0;

    /* Enable cover interrupt */
    DVD_COVER = 0x2;

    return TRUE;
}

/*
 * DVDLowStopMotor - Stop the DVD drive motor
 * 0x800A4850 | size: 0x8C
 */
BOOL DVDLowStopMotor(DVDCBCallback callback) {
    u32 timeout;

    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = 0;

    /* Send stop motor command */
    DVD_CMD = 0xE3000000;

    /* Start command */
    DVD_CONTROL = 0x1;

    /* Set up timeout alarm */
    timeout = (BUS_CLOCK / 4) * 10;
    OSCreateAlarm(&AlarmForTimeout_803FC2F8);
    OSSetAlarm(&AlarmForTimeout_803FC2F8, (s64)timeout, AlarmHandlerForTimeout);

    return TRUE;
}

/*
 * DVDLowReset - Reset the DVD drive
 * 0x800A4BC4 | size: 0xBC
 */
void DVDLowReset(void) {
    u32 resetReg;
    s64 startTime;
    u32 busSpeedQuarter;
    u32 waitTicks;

    /* Assert reset */
    DVD_COVER = 0x2;

    resetReg = DI_RESET_REG;
    DI_RESET_REG = (resetReg & ~0x4) | 0x1;

    /* Calculate wait time: ~12 ticks at bus clock / 4 / (1/2^15) */
    busSpeedQuarter = BUS_CLOCK / 4;
    waitTicks = (u32)((((u64)0x431CDE83ULL * busSpeedQuarter) >> 47) * 12) >> 3;

    startTime = __OSGetSystemTime();

    /* Busy-wait for the required time */
    while (1) {
        s64 elapsed = __OSGetSystemTime() - startTime;
        if (elapsed >= (s64)waitTicks) {
            break;
        }
    }

    /* Deassert reset */
    DI_RESET_REG = resetReg | 0x5;

    /* Mark reset occurred */
    ResetOccurred_8047A798 = 1;
    LastResetEnd_8047A790 = __OSGetSystemTime();
}

/*
 * __DVDLowSetWAType - Set the workaround type for the DVD drive
 * 0x800A4CAC | size: 0x44
 */
void __DVDLowSetWAType(u32 type, u32 location) {
    BOOL enabled = OSDisableInterrupts();
    WorkAroundType_8047A7A4 = type;
    WorkAroundSeekLocation_8047A7A8 = location;
    OSRestoreInterrupts(enabled);
}

/* fn_800A41D0 - 0x800A41D0 | size: 0x84 */
extern void fn_800A42C4(void);
extern void fn_800A46EC(void);
#if 1
asm void fn_800A41D0(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A41D0.inc"
}
#else
void fn_800A41D0(void) {}
#endif

/* fn_800A42C4 - 0x800A42C4 | size: 0x110 */
extern u32 lbl_8047A7C0;
extern u32 lbl_8047A7BC;
extern u32 lbl_8047A7B8;
extern u32 lbl_8047A784;
#if 1
asm void fn_800A42C4(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A42C4.inc"
}
#else
void fn_800A42C4(void* addr, u32 length, u32 offset, DVDCBCallback callback) {
    extern s64 __OSGetSystemTime(void);
    extern void OSCreateAlarm(OSAlarm* alarm);
    extern void OSSetAlarm(OSAlarm* alarm, s64 tick, OSAlarmHandler handler);
    extern WACommand CommandList_803FC290[];
    extern u32 lbl_8047A7C0;
    extern u32 lbl_8047A7BC;
    extern u32 lbl_8047A7B8;
    extern u32 lbl_8047A784;

    OSAlarm* alarm;
    u32 timeout;

    StopAtNextInt_8047A780 = 0;
    Callback_8047A788 = callback;
    alarm = (OSAlarm*)((u8*)CommandList_803FC290 + 0x68);
    lbl_8047A7C0 = 1;
    *(s64*)&lbl_8047A7B8 = __OSGetSystemTime();

    DVD_CMD = 0xA8000000;
    DVD_OFFSET_LO = offset >> 2;
    DVD_LENGTH = length;
    DVD_DMA_ADDR = (u32)addr;
    DVD_DMA_LEN = length;
    lbl_8047A784 = length;
    DVD_CONTROL = 3;

    if (length > 0xA00000) {
        timeout = (BUS_CLOCK / 4) * 20;
        OSCreateAlarm(alarm);
        OSSetAlarm(alarm, (s64)timeout, AlarmHandlerForTimeout);
    } else {
        timeout = (BUS_CLOCK / 4) * 10;
        OSCreateAlarm(alarm);
        OSSetAlarm(alarm, (s64)timeout, AlarmHandlerForTimeout);
    }
}
#endif

/* fn_800A43D4 - 0x800A43D4 | size: 0x80 */
#if 1
asm void fn_800A43D4(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A43D4.inc"
}
#else
void fn_800A43D4(void) {}
#endif

/* fn_800A4454 - 0x800A4454 | size: 0x298 */
extern void fn_800A7BCC(void);
extern u32 lbl_804789B8;
extern u32 lbl_8047A7B0;
extern u32 lbl_8047A7B4;
#if 1
asm void fn_800A4454(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4454.inc"
}
#else
void fn_800A4454(void) {}
#endif

/* fn_800A46EC - 0x800A46EC | size: 0x94 */
#if 1
asm void fn_800A46EC(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A46EC.inc"
}
#else
BOOL fn_800A46EC(u32 offset, DVDCBCallback callback) {
    u32 timeout;

    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = 0;

    DVD_CMD = 0xAB000000;
    DVD_OFFSET_LO = offset >> 2;
    DVD_CONTROL = 0x1;

    timeout = (BUS_CLOCK / 4) * 10;
    OSCreateAlarm(&AlarmForTimeout_803FC2F8);
    OSSetAlarm(&AlarmForTimeout_803FC2F8, (s64)timeout, AlarmHandlerForTimeout);

    return TRUE;
}
#endif

/* fn_800A47AC - 0x800A47AC | size: 0xA4 */
#if 1
asm void fn_800A47AC(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A47AC.inc"
}
#else
BOOL fn_800A47AC(void* addr, DVDCBCallback callback) {
    extern void OSCreateAlarm(OSAlarm* alarm);
    extern void OSSetAlarm(OSAlarm* alarm, s64 tick, OSAlarmHandler handler);
    extern OSAlarm AlarmForTimeout_803FC2F8;

    u32 timeout;

    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = 0;

    DVD_CMD = 0xA8000040;
    DVD_OFFSET_LO = 0;
    DVD_LENGTH = 0x20;
    DVD_DMA_ADDR = (u32)addr;
    DVD_DMA_LEN = 0x20;
    DVD_CONTROL = 3;

    timeout = (BUS_CLOCK / 4) * 10;
    OSCreateAlarm(&AlarmForTimeout_803FC2F8);
    OSSetAlarm(&AlarmForTimeout_803FC2F8, (s64)timeout, AlarmHandlerForTimeout);

    return TRUE;
}
#endif

/* fn_800A48DC - 0x800A48DC | size: 0x8C */
#if 1
asm void fn_800A48DC(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A48DC.inc"
}
#else
BOOL fn_800A48DC(DVDCBCallback callback) {
    u32 timeout;

    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = 0;

    /* Send seek command 0xE0000000 */
    DVD_CMD = 0xE0000000;

    /* Start command */
    DVD_CONTROL = 0x1;

    /* Set up timeout alarm */
    timeout = (BUS_CLOCK / 4) * 10;
    OSCreateAlarm(&AlarmForTimeout_803FC2F8);
    OSSetAlarm(&AlarmForTimeout_803FC2F8, (s64)timeout, AlarmHandlerForTimeout);

    return TRUE;
}
#endif

/* fn_800A4968 - 0x800A4968 | size: 0x9C */
#if 1
asm void fn_800A4968(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4968.inc"
}
#else
void fn_800A4968(void) {}
#endif

/* fn_800A4A04 - 0x800A4A04 | size: 0x98 */
#if 1
asm void fn_800A4A04(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4A04.inc"
}
#else
void fn_800A4A04(void) {}
#endif

/* fn_800A4A9C - 0x800A4A9C | size: 0x8C */
#if 1
asm void fn_800A4A9C(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4A9C.inc"
}
#else
void fn_800A4A9C(void) {}
#endif

/* fn_800A4B28 - 0x800A4B28 | size: 0x9C */
#if 1
asm void fn_800A4B28(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4B28.inc"
}
#else
void fn_800A4B28(void) {}
#endif

/* fn_800A4C80 - 0x800A4C80 | size: 0x14 */
extern u32 lbl_8047A7A0;
#if 0
asm void fn_800A4C80(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4C80.inc"
}
#else
BOOL fn_800A4C80(void) {
    StopAtNextInt_8047A780 = 1;
    lbl_8047A7A0 = 1;
    return TRUE;
}
#endif

/* fn_800A4C94 - 0x800A4C94 | size: 0x18 */
#if 0
asm void fn_800A4C94(void) {
#include "src/dolphin/dvd/DVDLow_fn_800A4C94.inc"
}
#else
DVDCBCallback fn_800A4C94(void) {
    DVDCBCallback cb;
    *(volatile u32*)0xCC006004 = 0;
    cb = Callback_8047A788;
    Callback_8047A788 = NULL;
    return cb;
}
#endif
