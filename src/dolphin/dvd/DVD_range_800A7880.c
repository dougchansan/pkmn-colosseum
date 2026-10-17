/**
 * @file DVD_range_800A7880.c
 * @brief Dolphin DVD cancellation and disk-state queries, 0x800A7880 -
 *        0x800A7CCC: DVDCancelAsync, DVDCancel, the cancel callback,
 *        DVDGetCurrentDiskID and DVDCheckDisk, with their switch tables
 *        (.data 0x80311B98 - 0x80311C00).
 *
 * Merged (lane D18) from sdk_candidate_800A7880.c, sdk_exact_800A7AFC.c and
 * sdk_range_800A7BD4.c. DVDCheckDisk's table sits at a four-byte-aligned
 * address (0x80311BCC) only because DVDCancelAsync's table precedes it in the
 * same object; linked alone its 8-aligned .data section shifted the DOL.
 */
#include "dolphin/types.h"
#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

extern u32 lbl_8047A808;
extern DVDCBCallback lbl_8047A80C;
extern u32 ResumeFromHere_8047A810;
extern DVDCommandBlock* executing_8047A7E8;
extern DVDCommandBlock DummyCommandBlock_803FC3A0;
extern void DVDLowBreak(void);
extern DVDLowCallback DVDLowClearCallback(void);
extern u32 __DVDDequeueWaitingQueue(u8* node);
extern void cbForStateMotorStopped_800A65A0(u32 interrupt);
extern void stateReady_800A6684(void);

BOOL DVDCancelAsync(DVDCommandBlock* block, DVDCBCallback callback)
{
    BOOL enabled;
    DVDLowCallback old;
    DVDCommandBlock* finished;

    enabled = OSDisableInterrupts();
    switch (block->state) {
    case -1:
    case 0:
    case 10:
        if (callback != NULL) {
            callback(0, block);
        }
        break;
    case 1:
        if (lbl_8047A808) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        lbl_8047A808 = TRUE;
        lbl_8047A80C = callback;
        if (block->command == 4 || block->command == 1) {
            DVDLowBreak();
        }
        break;
    case 2:
        __DVDDequeueWaitingQueue((u8*)block);
        block->state = 10;
        if (block->callback != NULL) {
            block->callback(-3, block);
        }
        if (callback != NULL) {
            callback(0, block);
        }
        break;
    case 3:
        switch (block->command) {
        case 5:
        case 4:
        case 13:
        case 15:
            if (callback != NULL) {
                callback(0, block);
            }
            break;
        default:
            if (lbl_8047A808) {
                OSRestoreInterrupts(enabled);
                return FALSE;
            }
            lbl_8047A808 = TRUE;
            lbl_8047A80C = callback;
            break;
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 11:
        old = DVDLowClearCallback();
        if (old != cbForStateMotorStopped_800A65A0) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        if (block->state == 4) {
            ResumeFromHere_8047A810 = 3;
        }
        if (block->state == 5) {
            ResumeFromHere_8047A810 = 4;
        }
        if (block->state == 6) {
            ResumeFromHere_8047A810 = 1;
        }
        if (block->state == 11) {
            ResumeFromHere_8047A810 = 2;
        }
        if (block->state == 7) {
            ResumeFromHere_8047A810 = 7;
        }
        finished = executing_8047A7E8;
        executing_8047A7E8 = &DummyCommandBlock_803FC3A0;
        block->state = 10;
        if (block->callback != NULL) {
            block->callback(-3, block);
        }
        if (callback != NULL) {
            callback(0, block);
        }
        stateReady_800A6684();
        break;
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

extern OSThreadQueue __DVDThreadQueue;

static void cbForCancelSync(s32 result, DVDCommandBlock* block);

BOOL DVDCancel(DVDCommandBlock* block) {
    BOOL enabled;

    if (!DVDCancelAsync(block, cbForCancelSync)) {
        return -1;
    }

    enabled = OSDisableInterrupts();
    for (;;) {
        s32 state = block->state;

        if ((u32)(state + 1) <= 1 || state == 10) {
            break;
        }
        if (state == 3) {
            u32 command = block->command;

            if ((u32)(command - 4) <= 1 || command == 13 || command == 15) {
                break;
            }
        }
        OSSleepThread(&__DVDThreadQueue);
    }
    OSRestoreInterrupts(enabled);
    return 0;
}

static void cbForCancelSync(s32 result, DVDCommandBlock* block) {
    OSWakeupThread(&__DVDThreadQueue);
}

void* DVDGetCurrentDiskID(void) {
    return (void*)0x80000000;
}

extern s32 FatalErrorFlag_8047A800;
extern s32 PausingFlag_8047A7F8;
extern volatile u32 __DIRegs[16] : 0xCC006000;

BOOL DVDCheckDisk(void) {
    BOOL enabled;
    s32 result;
    s32 state;
    u32 cover;

    enabled = OSDisableInterrupts();

    if (FatalErrorFlag_8047A800) {
        state = -1;
    } else if (PausingFlag_8047A7F8) {
        state = 8;
    } else if (executing_8047A7E8 == NULL) {
        state = 0;
    } else if (executing_8047A7E8 == &DummyCommandBlock_803FC3A0) {
        state = 0;
    } else {
        state = executing_8047A7E8->state;
    }

    switch (state) {
    case 1:
    case 2:
    case 9:
    case 10:
        result = TRUE;
        break;
    case -1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 11:
        result = FALSE;
        break;
    case 0:
    case 8:
        cover = __DIRegs[1];
        if (((cover >> 2) & 1) || (cover & 1)) {
            result = FALSE;
        } else if (ResumeFromHere_8047A810 != 0) {
            result = FALSE;
        } else {
            result = TRUE;
        }
        break;
    }

    OSRestoreInterrupts(enabled);
    return result;
}
