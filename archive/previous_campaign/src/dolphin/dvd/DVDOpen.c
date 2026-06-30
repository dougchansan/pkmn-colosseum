#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSAlarm.h"

/*
 * DVDOpen.c - DVD file open and read operations.
 *
 * Contains DVD file I/O functions that sit between DB.c and DVDLow.c
 * in the link order. These handle file open, read (sync and async),
 * seek, and directory operations.
 *
 * Matches: 0x800A2D38 - 0x800A3EB0
 *   fn_800A2D38 (0x2C) - DVDConvertEntrynumToPath helper
 *   fn_800A2D64 (0x34) - DVDConvertEntrynumToPath helper 2
 *   fn_800A2D98 (0xCC) - DVDConvertEntrynumToPath
 *   fn_800A2E64 (0x50) - DVD path helper
 *   fn_800A2EB4 (0xF8) - DVDReadAbsAsyncPrio
 *   fn_800A2FAC (0xC8) - DVDReadAbsAsyncForBS
 *   fn_800A3074 (0x70) - DVDReadDiskIDAsync
 *   fn_800A30E4 (0xB0) - cbForReadAsync
 *   fn_800A3194 (0xB0) - cbForSeekAsync
 *   fn_800A3244 (0x70) - DVDSeekAbsAsyncPrio
 *   fn_800A32B4 (0x34) - DVDGetTransferredSize
 *   fn_800A32E8 (0x4C) - DVDGetCurrentDiskID
 *   fn_800A3334 (0x28) - DVDGetFSTLocation
 *   fn_800A335C (0x58) - DVDSetAutoFatalMessaging
 *   fn_800A33B4 (0xA4) - DVDSetAutoInvalidation
 *   fn_800A3458 (0x18C) - __DVDGetCoverRegCallback
 *   fn_800A35E4 (0x94) - __DVDPrepareResetCallback
 *   fn_800A3678 (0xCC) - __DVDClearCoverInterrupt
 *   fn_800A3744 (0x88) - __DVDGetCoverStatus
 *   fn_800A37CC (0x54) - __DVDGetPreviousCoverStatus
 *   fn_800A3820 (0x54) - __DVDDisableCoverInterrupt
 *   fn_800A3874 (0x9C) - __DVDResetCoverCallback
 *   fn_800A3910 (0xD0) - __DVDUnrecoverableError
 *   fn_800A39E0 (0x98) - DVDSetUserData
 *   fn_800A3A78 (0x24) - DVDGetUserData
 *   fn_800A3A9C (0x24) - (getter)
 *   fn_800A3AC0 (0x1C) - (getter)
 *   fn_800A3ADC (0x44) - (setter)
 *   fn_800A3B20 (0x18) - (getter)
 *   fn_800A3B38 (0x44) - DVDPrepareStreamAbsAsync
 *   fn_800A3B7C (0x20) - DVDPrepareStreamAsync helper
 *   fn_800A3B9C (0x3C) - DVDCancelStreamAsync
 *   fn_800A3BD8 (0x28) - DVDCancelStream
 *   fn_800A3C00 (0x54) - DVDStopStreamAtEndAsync
 *   fn_800A3C54 (0x5C) - DVDGetStreamErrorStatusAsync
 *   fn_800A3CB0 (0x8C) - DVDGetStreamPlayAddrAsync
 *   fn_800A3D3C (0x174) - DVDGetStreamStartAddrAsync
 *   ... up to 0x800A3EB0
 */

extern void OSReport(const char* fmt, ...);
extern void DCInvalidateRange(void* addr, u32 size);

/*
 * DVDReadAbsAsyncPrio - Read from an absolute offset with priority.
 * 0x800A2EB4 | size: 0xF8
 */
BOOL DVDReadAbsAsyncPrio(DVDCommandBlock* block, void* addr, s32 length,
                         s32 offset, DVDCBCallback callback, s32 prio) {
    BOOL enabled;

    block->command = 1;  /* Read command */
    block->addr = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;
    block->state = 1;

    enabled = OSDisableInterrupts();

    /* Invalidate destination cache */
    DCInvalidateRange(addr, (u32)length);

    /* Queue the command */
    __DVDPushWaitingQueue(prio, block);

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * DVDReadAsyncPrio - Read from a file with priority.
 */
BOOL DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length,
                      s32 offset, DVDCBCallback callback, s32 prio) {
    /* Convert file-relative offset to absolute */
    return DVDReadAbsAsyncPrio(&fileInfo->cb, addr, length,
                                fileInfo->startAddr + offset,
                                callback, prio);
}

/*
 * DVDGetTransferredSize - Get the number of bytes transferred.
 * 0x800A32B4 | size: 0x34
 */
s32 DVDGetTransferredSize(DVDCommandBlock* block) {
    return block->transferredSize;
}

/*
 * DVDGetCurrentDiskID - Get the current disc ID.
 * 0x800A32E8 | size: 0x4C
 */
DVDDiskID* DVDGetCurrentDiskID(void) {
    return (DVDDiskID*)0x80000000;
}

/*
 * DVDSetAutoInvalidation - Enable/disable auto cache invalidation.
 * 0x800A33B4 | size: 0xA4
 */
static BOOL AutoInvalidation;

void DVDSetAutoInvalidation(BOOL enable) {
    AutoInvalidation = enable;
}

/*
 * DVDSetUserData - Set user data on a command block.
 * 0x800A39E0 | size: 0x98
 */
void DVDSetUserData(DVDCommandBlock* block, void* userData) {
    block->userData = userData;
}

/*
 * DVDGetUserData - Get user data from a command block.
 * 0x800A3A78 | size: 0x24
 */
void* DVDGetUserData(DVDCommandBlock* block) {
    return block->userData;
}

/*
 * DVDSeekAbsAsyncPrio - Seek to an absolute position.
 * 0x800A3244 | size: 0x70
 */
BOOL DVDSeekAbsAsyncPrio(DVDCommandBlock* block, s32 offset,
                         DVDCBCallback callback, s32 prio) {
    BOOL enabled;

    block->command = 2;  /* Seek command */
    block->offset = offset;
    block->callback = callback;
    block->state = 1;

    enabled = OSDisableInterrupts();
    __DVDPushWaitingQueue(prio, block);
    OSRestoreInterrupts(enabled);
    return TRUE;
}
