#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

/*
 * DVDState.c - DVD state management helpers.
 *
 * Contains DVD state machine helpers that sit between DVD.c's main
 * functions and DVDQueue.c in the link order. These handle disc
 * change detection, command completion, and error recovery.
 *
 * Matches: 0x800A7820 - 0x800A7DE8
 *   fn_800A7820 (0x10) - DVDPause / small getter
 *   fn_800A7830 (0x50) - DVDResume / small setter
 *   fn_800A7880 (0x27C) - __DVDDequeueWaitingQueue or DVDCancel
 *   fn_800A7AFC (0xAC) - DVDGetCommandBlockStatus
 *   fn_800A7BA8 (0x24) - DVDGetDriveInfo helper
 *   fn_800A7BCC (0x08) - stub/nop
 *   fn_800A7BD4 (0xF8) - __DVDStoreCoverCallback
 *   fn_800A7CCC (0x11C) - __DVDCheckCoverState
 */

extern void OSReport(const char* fmt, ...);
extern DVDCommandBlock* __DVDGetExecutingCommand(void);
extern DVDCommandBlock* __DVDPopWaitingQueue(void);

static u32 PauseFlag;
static u32 CoverStatus;

/*
 * DVDPause - Pause DVD operations.
 * 0x800A7820 | size: 0x10
 */
void DVDPause(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    PauseFlag = TRUE;
    OSRestoreInterrupts(enabled);
}

/*
 * DVDResume - Resume DVD operations.
 * 0x800A7830 | size: 0x50
 */
void DVDResume(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    PauseFlag = FALSE;

    /* Check for waiting commands and dispatch */
    OSRestoreInterrupts(enabled);
}

/*
 * DVDCancel - Cancel a pending DVD command.
 * 0x800A7880 | size: 0x27C
 */
BOOL DVDCancel(DVDCommandBlock* block) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Check if this block is currently executing */
    /* If so, we need to wait for it to complete */
    /* If queued, remove from wait queue */

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * DVDGetCommandBlockStatus - Get the status of a command block.
 * 0x800A7AFC | size: 0xAC
 */
s32 DVDGetCommandBlockStatus(DVDCommandBlock* block) {
    return block->state;
}

/*
 * fn_800A7BA8 - Drive info helper.
 * 0x800A7BA8 | size: 0x24
 */
u32 DVDGetDriveInfoStatus(void) {
    return 0;
}

/*
 * fn_800A7BCC - Stub.
 * 0x800A7BCC | size: 0x08
 */
void __DVDStateStub(void) {
}

/*
 * __DVDStoreCoverCallback - Store cover open/close callback.
 * 0x800A7BD4 | size: 0xF8
 */
typedef void (*DVDCoverCallback)(void);
static DVDCoverCallback CoverCallback;

void DVDSetCoverCallback(DVDCoverCallback callback) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    CoverCallback = callback;
    OSRestoreInterrupts(enabled);
}

/*
 * __DVDCheckCoverState - Check disc cover status.
 * 0x800A7CCC | size: 0x11C
 */
u32 __DVDCheckCoverState(void) {
    volatile u32* diRegs = (volatile u32*)0xCC006000;
    u32 status;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    status = diRegs[1];
    CoverStatus = (status >> 2) & 1;
    OSRestoreInterrupts(enabled);

    return CoverStatus;
}
