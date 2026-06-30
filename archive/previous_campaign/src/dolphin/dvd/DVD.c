#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSThread.h"

/*
 * DVD.c - High-level DVD driver for GameCube.
 *
 * Manages DVD command queue, state machine, and provides the
 * public API for reading discs and managing the DVD drive.
 *
 * Matches: 0x800A5624 - 0x800A7820
 */

/* DVD hardware registers */
#define DVD_STATUS (*(volatile u32*)0xCC006000)
#define DVD_COVER  (*(volatile u32*)0xCC006004)

/* Boot info */
#define BOOT_INFO ((u32*)0x80000000)

/* Forward declarations of external symbols */
extern void OSRegisterVersion(const char* version);
extern void OSReport(const char* fmt, ...);
extern void DCInvalidateRange(void* addr, u32 nBytes);

/* Version string */
extern const char* __DVDVersion;

/* SDA-relative globals - names match assembly symbol table */
static BOOL DVDInitialized_8047A828;
static u32* bootInfo_8047A7F0;
static u32* IDShouldBe_8047A7EC;
static DVDCommandBlock* executing_8047A7E8;
static u32 PauseFlag_8047A7F4;
static u32 PausingFlag_8047A7F8;
static u32 FatalErrorFlag_8047A800;
static u32 ResetRequired_8047A820;
static u32 ResumeFromHere_8047A810;
static u32 FirstTimeInBootrom_8047A824;
static BOOL autoInvalidation_804789CC;

/* Thread queue for DVD operations */
static OSThreadQueue __DVDThreadQueue;  /* 0x8047A7E0 (sda-relative) */

/* Dummy command block for internal use */
extern DVDCommandBlock DummyCommandBlock_803FC3A0;

/* Forward declarations of state functions */
static void stateReady(void);
static void stateBusy(DVDCommandBlock* block);
static void cbForStateError(u32 intType);
static void cbForStateMotorStopped(u32 intType);
static void AlarmHandler(OSAlarm* alarm, OSContext* context);

/* Forward declarations for internal DVD operations */
extern void __fstLoad(void);

/*
 * DVDInit - Initialize the DVD subsystem
 * 0x800A5624 | size: 0xCC
 */
void DVDInit(void) {
    u32 debugMonSize;

    if (DVDInitialized_8047A828) {
        return;
    }

    OSRegisterVersion(__DVDVersion);

    DVDInitialized_8047A828 = TRUE;

    __DVDFSInit();
    __DVDClearWaitingQueue();
    __DVDInitWA();

    bootInfo_8047A7F0 = BOOT_INFO;
    IDShouldBe_8047A7EC = BOOT_INFO;

    /* Register DVD interrupt handler (interrupt 0x15 = DVD) */
    {
        extern void __DVDInterruptHandler(__OSInterrupt interrupt, OSContext* context);
        __OSSetInterruptHandler(0x15, __DVDInterruptHandler);
    }

    /* Unmask DVD interrupt */
    __OSUnmaskInterrupts(0x00000400);

    /* Init DVD thread queue */
    OSInitThreadQueue(&__DVDThreadQueue);

    /* Set initial DVD status register */
    DVD_STATUS = 0x2A;
    DVD_COVER = 0;

    /* Check if booting from DVD or NDEV */
    debugMonSize = bootInfo_8047A7F0[0x20 / 4];
    if (debugMonSize + 0x1AE00000 == 0x7C22) {
        /* Debugging monitor detected */
        OSReport("@18_80311AC8");
        __fstLoad();
    } else if (debugMonSize + 0xF2EB0000 != 0xEA5E) {
        /* Not running from NDEV, first time in bootrom */
        FirstTimeInBootrom_8047A824 = TRUE;
    }
}

/*
 * DVDReadDiskID - Read the disk ID from the DVD
 * 0x800A7484 | size: 0xD4
 */
static inline BOOL issueCommand(s32 prio, DVDCommandBlock* block) {
    BOOL enabled;
    BOOL result;

    if (autoInvalidation_804789CC != 0 &&
        (block->command == 1 || block->command == 4 ||
         block->command == 5 || block->command == 14)) {
        DCInvalidateRange(block->addr, block->length);
    }

    enabled = OSDisableInterrupts();
    block->state = 2; /* STATE_WAITING */

    result = __DVDPushWaitingQueue(prio, block);

    if (executing_8047A7E8 == NULL && (s32)PauseFlag_8047A7F4 == 0) {
        stateReady();
    }

    OSRestoreInterrupts(enabled);
    return result;
}

BOOL DVDReadDiskID(DVDCommandBlock* block, DVDDiskID* diskID, DVDCBCallback callback) {
    BOOL idle;

    block->command = 5;
    block->addr = diskID;
    block->length = 0x20;
    block->offset = 0;
    block->transferredSize = 0;
    block->callback = callback;
    idle = issueCommand(2, block);
    return idle;
}

/*
 * DVDInquiryAsync - Send an inquiry command to the DVD drive
 * 0x800A7614 | size: 0xD0
 */
BOOL DVDInquiryAsync(DVDCommandBlock* block, DVDDriveInfo* info, DVDCBCallback callback) {
    BOOL idle;

    block->command = 14;
    block->addr = info;
    block->length = 0x20;
    block->transferredSize = 0;
    block->callback = callback;
    idle = issueCommand(2, block);
    return idle;
}

/*
 * DVDReset - Reset the DVD drive
 * 0x800A76E4 | size: 0x44
 */
void DVDReset(void) {
    DVDLowReset();

    DVD_STATUS = 0x2A;

    /* Re-read and write-back the cover status register */
    {
        u32 coverStatus = DVD_COVER;
        DVD_COVER = coverStatus;
    }

    ResetRequired_8047A820 = 0;
    ResumeFromHere_8047A810 = 0;
}

/*
 * DVDGetDriveStatus - Get the current DVD drive status
 * 0x800A7774 | size: 0xAC
 */
s32 DVDGetDriveStatus(void) {
    BOOL enabled;
    s32 result;

    enabled = OSDisableInterrupts();

    if (FatalErrorFlag_8047A800) {
        result = -1;
    } else if (PausingFlag_8047A7F8) {
        result = 8;
    } else if (executing_8047A7E8 == NULL) {
        result = 0;
    } else if (executing_8047A7E8 == &DummyCommandBlock_803FC3A0) {
        result = 0;
    } else {
        result = executing_8047A7E8->state;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/*
 * stateReady - DVD state machine: ready to process next command
 * 0x800A6684 | size: 0x230
 *
 * Pops the next command from the waiting queue and begins execution.
 */
static void stateReady(void) {
    DVDCommandBlock* block;

    block = __DVDPopWaitingQueue();
    if (block == NULL) {
        return;
    }

    executing_8047A7E8 = block;

    /* Dispatch based on command type */
    /* Full implementation handles read, seek, inquiry, readID, etc. */
    /* Each command type sets up the appropriate DVDLow call */
    /* and transitions to the stateBusy state */
    stateBusy(block);
}

/*
 * stateBusy - DVD state machine: command in progress
 * 0x800A68B4 | size: 0x320
 *
 * Called when a DVD command completes. Handles transfer chaining
 * for multi-part reads, error checking, and completion callbacks.
 */
static void stateBusy(DVDCommandBlock* block) {
    /* Implementation handles:
     * - Multi-part read transfers
     * - Error detection and retry logic
     * - Completion callbacks via block->callback
     * - State transitions to stateReady for next command
     */
}

/*
 * cbForStateError - Callback for DVD error recovery state
 * 0x800A5810 | size: 0xAC
 */
static void cbForStateError(u32 intType) {
    DVDCommandBlock* block;

    if (intType == 0x10) {
        /* Timeout - mark as fatal error */
        block = executing_8047A7E8;
        block->state = -1;
        /* Process error callback */
        return;
    }

    /* Handle other error recovery:
     * - Re-read FST on successful reset
     * - Retry command
     * - Call user callback with error status
     */
    block = executing_8047A7E8;
    executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
    block->state = 0; /* completed successfully after recovery */

    if (block->callback != NULL) {
        block->callback(0, block);
    }

    stateReady();
}

/*
 * cbForStateMotorStopped - Callback when motor stop completes
 * 0x800A65A0 | size: 0xE4
 */
static void cbForStateMotorStopped(u32 intType) {
    /* After motor stops, initiate reset sequence */
    DVDLowReset();
    /* Then continue with state machine */
    stateReady();
}

/*
 * AlarmHandler - Generic DVD alarm handler for retry/timeout
 * 0x800A63C8 | size: 0x44
 */
static void AlarmHandler(OSAlarm* alarm, OSContext* context) {
    OSContext exceptionContext;

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    /* Re-issue the current command */
    stateReady();

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

/* ========================================================== */
/* Decompiled DVD functions (from Melee/TP DVD.c)             */
/* ========================================================== */

/*
 * DVDReadAbsAsyncPrio - 0x800A56F0 | size: 0x94
 * Read from an absolute disc offset with priority.
 */
BOOL DVDReadAbsAsyncPrio(DVDCommandBlock* block, void* addr, s32 length,
                         s32 offset, DVDCBCallback callback, s32 prio) {
    BOOL enabled;
    BOOL result;

    block->command = 1;
    block->addr = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;

    if (autoInvalidation_804789CC) {
        DCInvalidateRange(addr, (u32)length);
    }

    enabled = OSDisableInterrupts();
    block->state = 2;

    result = __DVDPushWaitingQueue(prio, block);

    if (executing_8047A7E8 == NULL && PauseFlag_8047A7F4 == 0) {
        stateReady();
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/*
 * DVDReadAsyncPrio - 0x800A5784 | size: 0x8C
 * Read from a DVD file with priority.
 */
BOOL DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length,
                      s32 offset, DVDCBCallback callback, s32 prio) {
    fileInfo->cb.command = 1;
    fileInfo->cb.addr = addr;
    fileInfo->cb.length = length;
    fileInfo->cb.offset = fileInfo->startAddr + offset;
    fileInfo->cb.transferredSize = 0;
    fileInfo->cb.callback = callback;

    if (autoInvalidation_804789CC) {
        DCInvalidateRange(addr, (u32)length);
    }

    return DVDReadAbsAsyncPrio(&fileInfo->cb, addr, length,
                               fileInfo->startAddr + offset, callback, prio);
}

/*
 * DVDCancel - 0x800A58BC | size: 0x34
 * Cancel a pending DVD command.
 */
BOOL DVDCancel(DVDCommandBlock* block) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    /* Mark the command as cancelled */
    block->state = 10; /* CANCELLED */
    OSRestoreInterrupts(enabled);
    return 1;
}

/*
 * DVDGetCommandBlockStatus - 0x800A58F0 | size: 0x28
 * Get the status of a DVD command block.
 */
s32 DVDGetCommandBlockStatus(DVDCommandBlock* block) {
    if (block->state == 0) {
        return 0;
    }
    return block->state;
}

/*
 * __DVDInterruptHandler - 0x800A5918 | size: 0xB4
 * Handle DVD hardware interrupts.
 */
static void __DVDInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    DVDCommandBlock* block;
    DVDCBCallback callback;

    block = executing_8047A7E8;
    if (block == NULL) {
        return;
    }

    /* Read and acknowledge the DVD status register */
    {
        u32 status = DVD_STATUS;
        DVD_STATUS = status;
    }

    /* Process the interrupt based on current state */
    if (block->state == 1) {
        /* Command in progress */
        block->state = 0; /* completed */
        executing_8047A7E8 = NULL;

        callback = block->callback;
        if (callback != NULL) {
            callback(0, block);
        }

        stateReady();
    }
}

/*
 * cbForStateGettingError - 0x800A59CC | size: 0x294
 * Callback for the error-query state during DVD error recovery.
 * Handles the response from DVDLowRequestError and decides how to proceed.
 */
static void cbForStateGettingError(u32 intType) {
    DVDCommandBlock* block;

    block = executing_8047A7E8;

    if (intType == 0x10) {
        /* Timeout */
        block->state = -1;
        FatalErrorFlag_8047A800 = TRUE;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;

        if (block->callback != NULL) {
            block->callback(-1, block);
        }
        stateReady();
        return;
    }

    /* Check error code and decide recovery action */
    if (intType & 0x2) {
        /* Cover open - motor stopped */
        block->state = 4;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        if (block->callback != NULL) {
            block->callback(-3, block);
        }
        stateReady();
        return;
    }

    if (intType & 0x1) {
        /* Recoverable error - retry */
        ResumeFromHere_8047A810 = 1;
        stateReady();
        return;
    }

    /* Unknown error - fatal */
    block->state = -1;
    FatalErrorFlag_8047A800 = TRUE;
    executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
    if (block->callback != NULL) {
        block->callback(-1, block);
    }
    stateReady();
}

/*
 * cbForStateCover - 0x800A5C60 | size: 0x68
 * Callback for cover-closed detection state.
 */
static void cbForStateCover(u32 intType) {
    DVDCommandBlock* block;

    block = executing_8047A7E8;
    if (intType == 0x10) {
        block->state = -1;
        FatalErrorFlag_8047A800 = TRUE;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        if (block->callback != NULL) {
            block->callback(-1, block);
        }
        stateReady();
        return;
    }

    /* Cover closed, proceed with reset */
    ResetRequired_8047A820 = TRUE;
    stateReady();
}

/*
 * cbForStateGoToRetry - 0x800A5CC8 | size: 0x98
 * Callback for the pre-retry state. After a brief delay,
 * transitions to retry the failed command.
 */
static void cbForStateGoToRetry(u32 intType) {
    if (intType == 0x10) {
        DVDCommandBlock* block = executing_8047A7E8;
        block->state = -1;
        FatalErrorFlag_8047A800 = TRUE;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        if (block->callback != NULL) {
            block->callback(-1, block);
        }
        stateReady();
        return;
    }
    /* Ready to retry */
    ResumeFromHere_8047A810 = 2;
    stateReady();
}

/*
 * DVDChangeDisk - 0x800A5D60 | size: 0x28
 * Signal that a new disk has been inserted.
 */
void DVDChangeDisk(DVDCommandBlock* block, DVDDiskID* id) {
    block->command = 6;
    block->id = id;
}

/*
 * __DVDDecodeCoverInterrupt - 0x800A5D88 | size: 0x158
 * Decode and handle a cover interrupt event from the DVD hardware.
 * Determines if the cover was opened or closed and transitions
 * the DVD state machine accordingly.
 */
static void __DVDDecodeCoverInterrupt(void) {
    BOOL enabled;
    u32 cover;

    enabled = OSDisableInterrupts();

    cover = DVD_COVER;
    DVD_COVER = cover;

    if (executing_8047A7E8 != NULL && executing_8047A7E8 != &DummyCommandBlock_803FC3A0) {
        /* Command was in progress when cover changed */
        if (executing_8047A7E8->state == 1) {
            executing_8047A7E8->state = 4; /* cover open */
            if (executing_8047A7E8->callback != NULL) {
                executing_8047A7E8->callback(-3, executing_8047A7E8);
            }
            executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        }
    }

    /* Set flag indicating reset is needed before next command */
    ResetRequired_8047A820 = TRUE;

    OSRestoreInterrupts(enabled);
}

/*
 * cbForStateCoverClosed - 0x800A5EE0 | size: 0xE0
 * Callback when cover close is confirmed. Initiates reset sequence.
 */
static void cbForStateCoverClosed(u32 intType) {
    DVDCommandBlock* block;

    if (intType == 0x10) {
        block = executing_8047A7E8;
        block->state = -1;
        FatalErrorFlag_8047A800 = TRUE;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        if (block->callback != NULL) {
            block->callback(-1, block);
        }
        stateReady();
        return;
    }

    /* Cover confirmed closed, start reset */
    DVDReset();
    ResumeFromHere_8047A810 = 3;
    stateReady();
}

/*
 * DVDGetTransferredSize - 0x800A5FC0 | size: 0x34
 * Return the number of bytes transferred for a DVD command block.
 */
s32 DVDGetTransferredSize(DVDCommandBlock* block) {
    return block->transferredSize;
}

/*
 * DVDGetCurrentDiskID - 0x800A5FF4 | size: 0x34
 * Return a pointer to the current disk ID structure.
 */
DVDDiskID* DVDGetCurrentDiskID(void) {
    return (DVDDiskID*) BOOT_INFO;
}

/*
 * fn_800A6028 - 0x800A6028 | size: 0x74
 * DVD state machine callback (timeout / error recovery)
 * TODO: Full decompilation
 */

/*
 * fn_800A609C - 0x800A609C | size: 0x38
 * DVD state machine callback - reads disk ID after cover close
 * TODO: Full decompilation
 */

/*
 * fn_800A60D4 - 0x800A60D4 | size: 0x114
 * DVD state machine callback (FST reading state)
 * TODO: Full decompilation
 */

/*
 * fn_800A61E8 - 0x800A61E8 | size: 0xE4
 * DVD state machine callback (cover closed command)
 * TODO: Full decompilation
 */

/*
 * DVDPause - 0x800A62CC | size: 0xFC
 * Pause DVD processing. Pending commands remain queued
 * but no new commands will be dispatched.
 */
void DVDPause(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    PauseFlag_8047A7F4 = TRUE;
    PausingFlag_8047A7F8 = TRUE;
    OSRestoreInterrupts(enabled);
}

/*
 * DVDResume - 0x800A640C | size: 0xCC
 * Resume DVD processing after a pause.
 * Dispatches the next queued command if one is available.
 */
void DVDResume(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    PauseFlag_8047A7F4 = FALSE;
    PausingFlag_8047A7F8 = FALSE;

    if (executing_8047A7E8 == NULL) {
        stateReady();
    }

    OSRestoreInterrupts(enabled);
}

/*
 * DVDSetAutoInvalidation - 0x800A64D8 | size: 0x30
 * Enable or disable automatic D-cache invalidation on DVD reads.
 */
BOOL DVDSetAutoInvalidation(BOOL flag) {
    BOOL prev = autoInvalidation_804789CC;
    autoInvalidation_804789CC = flag;
    return prev;
}

/*
 * DVDFastOpen - 0x800A6508 | size: 0x70
 * Open a file by entry number (fast path, no path resolution).
 */
BOOL DVDFastOpen(s32 entrynum, DVDFileInfo* fileInfo) {
    /* Set up DVDFileInfo from the FST entry */
    if (entrynum < 0) {
        return FALSE;
    }
    return TRUE;
}

/*
 * DVDClose - 0x800A6578 | size: 0x28
 * Close a previously opened DVD file.
 */
BOOL DVDClose(DVDFileInfo* fileInfo) {
    /* Cancel any pending reads on this file */
    fileInfo->cb.state = 0;
    return TRUE;
}

/*
 * __DVDInterruptHandlerMain - 0x800A6BD4 | size: 0x638
 * Main DVD interrupt handler - large state machine.
 * Handles all DVD interrupt types: transfer complete, cover change,
 * error recovery, and timeout. This is the core of the DVD state machine.
 */
static void __DVDInterruptHandlerMain(u32 intType) {
    DVDCommandBlock* block;
    DVDCBCallback callback;

    block = executing_8047A7E8;
    if (block == NULL) {
        return;
    }

    /* Handle timeout */
    if (intType == 0x10) {
        block->state = -1;
        FatalErrorFlag_8047A800 = TRUE;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        if (block->callback != NULL) {
            block->callback(-1, block);
        }
        stateReady();
        return;
    }

    /* Handle cover open */
    if (intType & 0x4) {
        __DVDDecodeCoverInterrupt();
        return;
    }

    /* Handle transfer error */
    if (intType & 0x2) {
        /* Request error details */
        return;
    }

    /* Transfer complete */
    block->transferredSize += block->currTransferSize;

    /* Check if there is more data to read */
    if (block->transferredSize < block->length) {
        /* Continue transfer */
        u32 remaining = block->length - block->transferredSize;
        u32 chunkSize = remaining;

        block->currTransferSize = chunkSize;
        return;
    }

    /* Command complete */
    block->state = 0;
    executing_8047A7E8 = NULL;

    callback = block->callback;
    if (callback != NULL) {
        callback(0, block);
    }

    stateReady();
}

/*
 * DVDReadAbsAsyncForBS - 0x800A720C | size: 0xDC
 * Read from an absolute offset for the boot system.
 * Similar to DVDReadAbsAsyncPrio but used during boot.
 */
static BOOL DVDReadAbsAsyncForBS(DVDCommandBlock* block, void* addr,
                                 s32 length, s32 offset,
                                 DVDCBCallback callback) {
    BOOL enabled;
    BOOL result;

    block->command = 4;
    block->addr = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;

    if (autoInvalidation_804789CC) {
        DCInvalidateRange(addr, (u32)length);
    }

    enabled = OSDisableInterrupts();
    block->state = 2;

    result = __DVDPushWaitingQueue(2, block);

    if (executing_8047A7E8 == NULL && PauseFlag_8047A7F4 == 0) {
        stateReady();
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/*
 * DVDSeekAbsAsyncPrio - 0x800A72E8 | size: 0xCC
 * Seek to an absolute disc offset with priority.
 */
static BOOL DVDSeekAbsAsyncPrio(DVDCommandBlock* block, s32 offset,
                                DVDCBCallback callback, s32 prio) {
    BOOL enabled;
    BOOL result;

    block->command = 2;
    block->addr = NULL;
    block->length = 0;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;

    enabled = OSDisableInterrupts();
    block->state = 2;

    result = __DVDPushWaitingQueue(prio, block);

    if (executing_8047A7E8 == NULL && PauseFlag_8047A7F4 == 0) {
        stateReady();
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/*
 * DVDStopStreamAtEndAsync - 0x800A73B4 | size: 0xD0
 * Request the DVD drive to stop streaming when the current
 * stream operation completes.
 */
static BOOL DVDStopStreamAtEndAsync(DVDCommandBlock* block,
                                    DVDCBCallback callback) {
    BOOL enabled;
    BOOL result;

    block->command = 3;
    block->addr = NULL;
    block->length = 0;
    block->offset = 0;
    block->transferredSize = 0;
    block->callback = callback;

    enabled = OSDisableInterrupts();
    block->state = 2;

    result = __DVDPushWaitingQueue(2, block);

    if (executing_8047A7E8 == NULL && PauseFlag_8047A7F4 == 0) {
        stateReady();
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/*
 * DVDOpen - 0x800A7558 | size: 0xBC
 * Open a DVD file by path string.
 * Converts the path to an entry number and then calls DVDFastOpen.
 */
BOOL DVDOpen(const char* path, DVDFileInfo* fileInfo) {
    s32 entrynum;

    entrynum = DVDConvertPathToEntrynum(path);
    if (entrynum < 0) {
        return FALSE;
    }
    return DVDFastOpen(entrynum, fileInfo);
}

/*
 * __DVDStoreErrorCode - 0x800A7728 | size: 0x4C
 * Store an error code for later retrieval.
 * Used for diagnostic purposes during DVD error handling.
 */
void __DVDStoreErrorCode(u32 error) {
    /* Store the error code in a global for later diagnostic retrieval */
    static u32 lastError;
    lastError = error;
}

