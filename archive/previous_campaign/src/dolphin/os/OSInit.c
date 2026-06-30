#include "dolphin/os/OS.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSMemory.h"
#include "dolphin/os/OSReset.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"
#include "dolphin/dvd/dvd.h"
#include "dolphin/exi/EXI.h"
#include "dolphin/si/SI.h"

extern void* memset(void* dest, int val, u32 n);
extern void* memcpy(void* dest, const void* src, u32 n);

/*
 * OSInit.c - Dolphin OS initialization.
 *
 * Adapted from doldecomp/melee matching implementation.
 *
 * Matches: 0x800998E0 - 0x8009A27C
 */

extern u32 __DVDLongFileNameFlag;
extern u32 __PADSpec;
extern u8  __ArenaLo[];
extern u8  _stack_addr[];
extern u8  __ArenaHi[];

extern const char* __OSVersion;

extern void OSExceptionInit(void);
extern void __OSInitAudioSystem(void);
extern void __OSInitSystemCall(void);
extern void __OSInitSram(void);
extern void __OSModuleInit(void);
extern void EnableMetroTRKInterrupts(void);

static BOOL AreWeInitialized;

typedef struct OSBootInfo_s {
    u8  diskID[0x20];
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void* arenaLo;
    void* arenaHi;
    u32 FSTLocation;
    u32 FSTMaxLength;
} OSBootInfo_s;

static OSBootInfo_s* BootInfo;
static u32*  BI2DebugFlag;
static u32   BI2DebugFlagHolder;

u32   __OSInIPL;
u32   __OSIsGcam;
s64   __OSStartTime;

static DVDDriveInfo DriveInfo;
static DVDCommandBlock DriveBlock;

#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + 0x80000000))

static void ClearArena(void);
static void InquiryCallback(s32 result, DVDCommandBlock* block);

u32 OSGetConsoleType(void) {
    if ((!BootInfo) || (BootInfo->consoleType == 0)) {
        return 0x10000002;
    }
    return BootInfo->consoleType;
}

#define BOOT_REGION_START (*(u32*)0x812FDFF0)
#define BOOT_REGION_END   (*(u32*)0x812FDFEC)

static void ClearArena(void) {
    if (OSGetResetCode() != 0x80000000) {
        memset(OSGetArenaLo(), 0, (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
    } else {
        u32 boot_region_start = BOOT_REGION_START;
        u32 boot_region_end = BOOT_REGION_END;

        if (boot_region_start == 0) {
            memset(OSGetArenaLo(), 0,
                   (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
        } else if ((u32)OSGetArenaLo() < boot_region_start) {
            if ((u32)OSGetArenaHi() <= boot_region_start) {
                memset(OSGetArenaLo(), 0,
                       (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
            } else {
                memset(OSGetArenaLo(), 0,
                       boot_region_start - (u32)OSGetArenaLo());
                if ((u32)OSGetArenaHi() > boot_region_end) {
                    memset((void*)boot_region_end, 0,
                           (u32)OSGetArenaHi() - boot_region_end);
                }
            }
        }
    }
}

static void InquiryCallback(s32 result, DVDCommandBlock* block) {
    if (result != 0) {
        *(volatile u16*)0x800030E6 = 1;
    } else {
        u16 deviceCode = DriveInfo.deviceCode;
        *(volatile u16*)0x800030E6 = deviceCode | 0x8000;
    }
}

void OSInit(void) {
    u32 consoleType;
    u32 bi2StartAddr;

    if (AreWeInitialized != 0) {
        return;
    }
    AreWeInitialized = TRUE;
    __OSStartTime = __OSGetSystemTime();
    OSDisableInterrupts();

    PPCMtmmcr0(0);
    PPCMtmmcr1(0);
    PPCMtpmc1(0);
    PPCMtpmc2(0);
    PPCMtpmc3(0);
    PPCMtpmc4(0);
    PPCDisableSpeculation();
    PPCSetFpNonIEEEMode();

    BootInfo = (OSBootInfo_s*)OSPhysicalToCached(0);
    BI2DebugFlag = NULL;
    __DVDLongFileNameFlag = 0;

    bi2StartAddr = *(u32*)OSPhysicalToCached(0xF4);
    if (bi2StartAddr) {
        BI2DebugFlag = (u32*)((u8*)bi2StartAddr + 0xC);
        __PADSpec = ((u32*)bi2StartAddr)[9];
        *(u8*)OSPhysicalToCached(0x30E8) = (u8)*BI2DebugFlag;
        *(u8*)OSPhysicalToCached(0x30E9) = (u8)__PADSpec;
    } else if (*(u32*)OSPhysicalToCached(0x34) != 0) {
        bi2StartAddr = *(u8*)OSPhysicalToCached(0x30E8);
        BI2DebugFlagHolder = bi2StartAddr;
        BI2DebugFlag = &BI2DebugFlagHolder;
        __PADSpec = *(u8*)OSPhysicalToCached(0x30E9);
    }

    __DVDLongFileNameFlag = 1;

    OSSetArenaLo((!BootInfo->arenaLo) ? (void*)__ArenaLo : BootInfo->arenaLo);
    if ((!BootInfo->arenaLo) && (BI2DebugFlag) && (*BI2DebugFlag < 2)) {
        OSSetArenaLo((void*)(((u32)_stack_addr + 0x1F) & ~0x1F));
    }
    OSSetArenaHi((!BootInfo->arenaHi) ? (void*)__ArenaHi : BootInfo->arenaHi);

    OSExceptionInit();
    __OSInitSystemCall();
    OSInitAlarm();
    __OSModuleInit();
    __OSInterruptInit();
    __OSSetInterruptHandler(0x16, (__OSInterruptHandler)__OSResetSWInterruptHandler);
    __OSContextInit();
    __OSCacheInit();
    EXIInit();
    SIInit();
    __OSInitSram();
    __OSThreadInit();
    __OSInitAudioSystem();

    PPCMthid2(PPCMfhid2() & 0xBFFFFFFF);

    if (!__OSInIPL) {
        __OSInitMemoryProtection();
    }

    OSReport("Dolphin OS $Revision: 54 $.\n");
    OSReport("Kernel built : %s %s\n", __DATE__, __TIME__);
    OSReport("Console Type : ");

    consoleType = OSGetConsoleType();
    if ((consoleType & 0x10000000) == 0x10000000) {
        OSReport("Retail %d\n", consoleType);
    } else {
        switch (consoleType) {
            case 0x00000000:
                OSReport("Mac Emulator\n");
                break;
            case 0x10000000:
                OSReport("PC Emulator\n");
                break;
            case 0x10000002:
                OSReport("EPPC Arthur\n");
                break;
            case 0x10000003:
                OSReport("EPPC Minnow\n");
                break;
            default:
                OSReport("Development HW%d\n", ((u32)consoleType - 0x10000000) - 3);
                break;
        }
    }

    OSReport("Memory %d MB\n", (u32)BootInfo->memorySize >> 20);
    OSReport("Arena : 0x%x - 0x%x\n", OSGetArenaLo(), OSGetArenaHi());

    if (__OSVersion) {
        OSRegisterVersion(__OSVersion);
    }

    if (BI2DebugFlag != NULL && *BI2DebugFlag >= 2) {
        EnableMetroTRKInterrupts();
    }

    ClearArena();
    OSEnableInterrupts();

    if (__OSInIPL) {
        return;
    }

    DVDInit();

    if (__OSIsGcam) {
        *(volatile u16*)0x800030E6 = 0x9000;
        return;
    }

    DCInvalidateRange(&DriveInfo, sizeof(DVDDriveInfo));
    DVDInquiryAsync(&DriveBlock, &DriveInfo, InquiryCallback);
}

void OSRegisterVersion(const char* id) {
    OSReport("%s\n", id);
}

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 1 function(s)
 * =================================================================== */

/* fn_8009A23C - 0x8009A23C | size: 0x14 */
#pragma push
#pragma peephole off
u32 fn_8009A23C(void) {
    asm {
        lis r3, 0xcc00
        addi r3, r3, 0x6000
        lwz r0, 0x24(r3)
        clrlwi r3, r0, 24
    }
}
#pragma pop
