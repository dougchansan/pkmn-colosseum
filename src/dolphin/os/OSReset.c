#include "dolphin/os/OSReset.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"
#include "dolphin/dvd/dvd.h"

/* SDA symbol aliases used by stub functions */
extern OSResetFunctionInfo* ResetFunctionQueue_8047A738;

/*
 * OSReset.c - System reset and reboot functionality.
 *
 * Adapted from doldecomp/melee and zeldaret/tp matching implementations.
 *
 * Matches: 0x8009FAF8 - 0x800A03B4
 */

#define ENQUEUE_INFO(info, queue)                            \
    do {                                                     \
        OSResetFunctionInfo* __prev = (queue)->tail;         \
        if (__prev == 0) {                                   \
            (queue)->head = (info);                          \
        } else {                                             \
            __prev->next = (info);                           \
        }                                                    \
        (info)->prev = __prev;                               \
        (info)->next = 0;                                    \
        (queue)->tail = (info);                              \
    } while(0);

#define ENQUEUE_INFO_PRIO(info, queue)               \
    do {                                             \
        OSResetFunctionInfo* __prev;                 \
        OSResetFunctionInfo* __next;                 \
        for(__next = (queue)->head; __next           \
          && (__next->priority <= (info)->priority); \
                __next = __next->next) ;             \
                                                     \
        if (__next == 0) {                           \
            ENQUEUE_INFO(info, queue);               \
        } else {                                     \
            (info)->next = __next;                   \
            __prev = __next->prev;                   \
            __next->prev = (info);                   \
            (info)->prev = __prev;                   \
            if (__prev == 0) {                       \
                (queue)->head = (info);              \
            } else {                                 \
                __prev->next = (info);               \
            }                                        \
        }                                            \
    } while(0);

typedef struct OSResetFunctionQueue {
    OSResetFunctionInfo* head;
    OSResetFunctionInfo* tail;
} OSResetFunctionQueue;

#if defined(OSRESET_SUFFIX)
#define ResetFunctionQueue (*(OSResetFunctionQueue*)&ResetFunctionQueue_8047A738)
#else
static OSResetFunctionQueue ResetFunctionQueue;
#endif

extern void __OSReboot(u32 resetCode, u32 bootDol);
extern void __OSStopAudioSystem(void);
extern BOOL __OSSyncSram(void);
extern void* __OSLockSram(void);
extern void __OSUnlockSram(BOOL commit);
extern BOOL __PADDisableRecalibration(BOOL disable);
extern void LCDisable(void);
extern void ICFlashInvalidate(void);
extern void* memset(void* dest, int val, u32 n);

typedef struct ApploaderHeader {
    char date[16];
    u32 entry;
    u32 size;
    u32 rebootSize;
    u32 reserved2;
} ApploaderHeader;

#define AT_ADDRESS(addr) : addr

extern ApploaderHeader lbl_803FB820;
extern void* lbl_8047A728;
extern void* lbl_8047A72C;
extern volatile BOOL lbl_8047A730[2];

extern u32 OSReboot_817FFFF8 AT_ADDRESS(0x817FFFF8);
extern u32 OSReboot_817FFFFC AT_ADDRESS(0x817FFFFC);
extern u32 OSReboot_812FDFF0 AT_ADDRESS(0x812FDFF0);
extern u32 OSReboot_812FDFEC AT_ADDRESS(0x812FDFEC);
extern u8 OSReboot_800030E2 AT_ADDRESS(0x800030E2);
extern u32 __OSBusClock AT_ADDRESS(0x800000F8);
extern s32 __OSIsGcam;

extern s32 DVDSetAutoInvalidation(s32 enable);
extern DVDDiskID* DVDGetCurrentDiskID(void);
extern void fn_8009FAEC(void);
extern void fn_8009FADC(void* address);
extern void DVDResume(void);
extern BOOL DVDCheckDisk(void);
extern void __DVDPrepareResetAsync(DVDCBCallback callback);
extern BOOL DVDCancelStreamAsync(DVDCommandBlock* block, DVDCBCallback callback);
extern void AISetStreamVolLeft(u32 volume);
extern void AISetStreamVolRight(u32 volume);
extern void AISetStreamPlayState(u32 state);
void __OSDoHotReset(u32 resetCode);

volatile u16 __VIRegs[59] AT_ADDRESS(0xCC002000);
#define __PIRegs     ((volatile u32*)0xCC003000)

#define OS_BUS_CLOCK   __OSBusClock
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)

#define OS_INTERRUPTMASK_PI_RSW 0x200

#if defined(OSRESET_SUFFIX)
static inline int CallResetFunctions(int final);
extern void Reset_8009FF50(u32 resetCode);
#elif defined(OSRESET_PREFIX)
s32 __OSCallResetFunctions(s32 final);
#else
static int CallResetFunctions(int final);
#endif
static void CancelThreads(void);

#define OS_ROUND_UP_32B(value) (((u32)(value) + 0x1F) & ~0x1F)

static inline BOOL IsStreamEnabled(void) {
    if (DVDGetCurrentDiskID()->streaming) {
        return TRUE;
    }
    return FALSE;
}

#if !defined(OSRESET_SUFFIX)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void __OSReboot(u32 resetCode, u32 bootDol) {
    OSContext exceptionContext;
    DVDCommandBlock streamCommand;
    DVDCommandBlock headerCommand;
    DVDCommandBlock rebootCommand;
    ApploaderHeader* header = &lbl_803FB820;
    u32 rebootSize;
    u32 offset;
    OSTime start;

    OSDisableInterrupts();

    OSReboot_817FFFFC = 0;
    OSReboot_817FFFF8 = 0;
    OSReboot_800030E2 = TRUE;
    OSReboot_812FDFF0 = (u32)lbl_8047A728;
    OSReboot_812FDFEC = (u32)lbl_8047A72C;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    DVDInit();
    DVDSetAutoInvalidation(TRUE);
    DVDResume();

    lbl_8047A730[0] = FALSE;
    __DVDPrepareResetAsync((DVDCBCallback)fn_8009FAEC);
    __OSMaskInterrupts(0xFFFFFFE0);
    __OSUnmaskInterrupts(0x400);
    OSEnableInterrupts();

    start = OSGetTime();
    while (lbl_8047A730[0] != TRUE) {
        if (!DVDCheckDisk() || OS_TIMER_CLOCK < OSGetTime() - start) {
            __OSDoHotReset(OSReboot_817FFFFC);
        }
    }

    if (!__OSIsGcam && IsStreamEnabled()) {
        AISetStreamVolLeft(0);
        AISetStreamVolRight(0);
        DVDCancelStreamAsync(&streamCommand, NULL);

        start = OSGetTime();
        while (DVDGetCommandBlockStatus(&streamCommand)) {
            if (!DVDCheckDisk() || OS_TIMER_CLOCK < OSGetTime() - start) {
                __OSDoHotReset(OSReboot_817FFFFC);
            }
        }

        AISetStreamPlayState(0);
    }

    DVDReadAbsAsyncPrio(&headerCommand, header, sizeof(ApploaderHeader),
                        0x2440, NULL, 0);
    start = OSGetTime();
    while (DVDGetCommandBlockStatus(&headerCommand)) {
        if (!DVDCheckDisk() || OS_TIMER_CLOCK < OSGetTime() - start) {
            __OSDoHotReset(OSReboot_817FFFFC);
        }
    }

    offset = header->size + 0x20;
    rebootSize = OS_ROUND_UP_32B(header->rebootSize);
    DVDReadAbsAsyncPrio(&rebootCommand, (void*)0x81300000, rebootSize,
                        offset + 0x2440, NULL, 0);
    start = OSGetTime();
    while (DVDGetCommandBlockStatus(&rebootCommand)) {
        if (!DVDCheckDisk() || OS_TIMER_CLOCK < OSGetTime() - start) {
            __OSDoHotReset(OSReboot_817FFFFC);
        }
    }

    ICInvalidateRange((void*)0x81300000, rebootSize);
    OSDisableInterrupts();
    ICFlashInvalidate();
    fn_8009FADC((void*)0x81300000);
}
#pragma pop

void OSRegisterResetFunction(OSResetFunctionInfo* info) {
    ENQUEUE_INFO_PRIO(info, &ResetFunctionQueue);
}
#endif

#if defined(OSRESET_SUFFIX)
static inline int CallResetFunctions(int final) {
#elif defined(OSRESET_PREFIX)
s32 __OSCallResetFunctions(s32 final) {
#else
static int CallResetFunctions(int final) {
#endif
    OSResetFunctionInfo* info;
    int err;

    err = 0;
    info = ResetFunctionQueue.head;

    while (info != NULL && err == 0) {
        err |= !info->func(final);
        info = info->next;
    }

    err |= !__OSSyncSram();

    if (err != 0) {
        return 0;
    }
    return 1;
}

#if !defined(OSRESET_PREFIX) && !defined(OSRESET_SUFFIX)
static asm void Reset(u32 resetCode) {
    nofralloc
    b       _skip1
_cache:
    mfspr   r8, HID0
    ori     r8, r8, 0x8
    mtspr   HID0, r8
    isync
    sync
    nop
    b       _wait
_skip1:
    b       _skip2
_wait:
    mftb    r5, 268
_waitloop:
    mftb    r6, 268
    subf    r7, r5, r6
    cmplwi  r7, 0x1124
    blt     _waitloop
    nop
    b       _reset
_skip2:
    b       _skip3
_reset:
    lis     r8, 0xCC00
    ori     r8, r8, 0x3000
    li      r4, 0x3
    stw     r4, 0x24(r8)
    stw     r3, 0x24(r8)
    nop
    b       _hang
_skip3:
    b       _hang2
_hang:
    nop
    b       _hang
_hang2:
    b       _cache
}
#endif

#if !defined(OSRESET_PREFIX)
static inline void CancelThreads(void) {
    OSThread* thread;
    OSThread* next;

    for (thread = ((OSThreadQueue*)0x800000DC)->head; thread != NULL; thread = next) {
        next = thread->linkActive.next;
        switch (thread->state) {
        case 1:
        case 4:
            OSCancelThread(thread);
            break;
        default:
            break;
        }
    }
}

#pragma push
#pragma peephole off
void __OSDoHotReset(u32 resetCode) {
    OSDisableInterrupts();
    __VIRegs[1] = 0;
    ICFlashInvalidate();
#if defined(OSRESET_SUFFIX)
    Reset_8009FF50(resetCode * 8);
#else
    Reset(resetCode * 8);
#endif
}
#pragma pop

#pragma push
#pragma peephole off
void OSResetSystem(BOOL reset, u32 resetCode, BOOL forceMenu) {
    BOOL rc;
    BOOL padcal;
    u32 unused[3];

    OSDisableScheduler();
    __OSStopAudioSystem();

    if (reset == 2) {
        padcal = __PADDisableRecalibration(TRUE);
    }

#if defined(OSRESET_SUFFIX)
    do {} while (CallResetFunctions(0) == 0);
#else
    do {} while (__OSCallResetFunctions(0) == 0);
#endif

    if (reset == 1 && forceMenu != 0) {
        void* sram;
        sram = __OSLockSram();
        *(u8*)((u8*)sram + 0x13) |= 0x40;
        __OSUnlockSram(TRUE);
        do {} while (__OSSyncSram() == 0);
    }

    OSDisableInterrupts();
#if defined(OSRESET_SUFFIX)
    CallResetFunctions(1);
#else
    __OSCallResetFunctions(1);
#endif
    LCDisable();

    if (reset == 1) {
        OSDisableInterrupts();
        __VIRegs[1] = 0;
        ICFlashInvalidate();
#if defined(OSRESET_SUFFIX)
        Reset_8009FF50(resetCode * 8);
#else
        Reset(resetCode * 8);
#endif
    } else if (reset == 0) {
        CancelThreads();
        OSEnableScheduler();
        __OSReboot(resetCode, forceMenu);
    }

    CancelThreads();

    memset((void*)0x80000040, 0, 0x8C);
    memset((void*)0x800000D4, 0, 0x14);
    memset((void*)0x800000F4, 0, 4);
    memset((void*)0x80003000, 0, 0xC0);
    memset((void*)0x800030C8, 0, 0xC);
    memset((void*)0x800030E2, 0, 1);

    __PADDisableRecalibration(padcal);
}
#pragma pop

#pragma push
#pragma peephole off
u32 OSGetResetCode(void) {
    u32 resetCode;
    volatile u32* pi;

    if (*(volatile u8*)0x800030E2 != 0) {
        resetCode = 0x80000000;
    } else {
        resetCode = ((pi = (volatile u32*)0xCC003000)[9] & 0xFFFFFFF8) / 8;
    }
    return resetCode;
}
#pragma pop

typedef void (*OSResetSWCallback)(void);

#if defined(OSRESET_SUFFIX)
extern OSResetSWCallback lbl_8047A740;
extern BOOL lbl_8047A744;
extern BOOL lbl_8047A748;
extern OSTime lbl_8047A758;
#define ResetCallback lbl_8047A740
#define Down lbl_8047A744
#define LastState lbl_8047A748
#define HoldDown lbl_8047A758
#else
static OSResetSWCallback ResetCallback;
static BOOL Down;
static BOOL LastState;
static OSTime HoldUp;
static OSTime HoldDown;
#endif

void __OSResetSWInterruptHandler(s16 interrupt, OSContext* context) {
    OSResetSWCallback callback;
    u32 timeout;

    HoldDown = __OSGetSystemTime();
    timeout = OSMicrosecondsToTicks(100);
    while (__OSGetSystemTime() - HoldDown < timeout &&
           !(__PIRegs[0] & 0x00010000)) {
        ;
    }
    if (!(__PIRegs[0] & 0x00010000)) {
        LastState = Down = TRUE;
        __OSMaskInterrupts(OS_INTERRUPTMASK_PI_RSW);
        if (ResetCallback) {
            callback = ResetCallback;
            ResetCallback = NULL;
            callback();
        }
    }
    __PIRegs[0] = 2;
}
#endif

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 2 function(s)
 * =================================================================== */

/* __OSCallResetFunctions - 0x8009FEBC | size: 0x94
 * CallResetFunctions variant - walks the reset function queue, calling
 * each function. Stops early if any function fails. Returns 1 on
 * success, 0 if any function (or SyncSram) failed.
 */
/* __OSDoHotReset (0x8009FFC0) defined above with the OSResetSystem cluster. */
