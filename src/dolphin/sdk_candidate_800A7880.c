/* Residual Dolphin asynchronous DVD cancellation helper. */
#include "src/dolphin/sdk_range_800A7820.c"

extern u32 lbl_8047A808;
extern DVDCBCallback lbl_8047A80C;
extern u32 ResumeFromHere_8047A810;
extern DVDCommandBlock* executing_8047A7E8;
extern DVDCommandBlock DummyCommandBlock_803FC3A0;
extern void fn_800A4C80(void);
extern DVDLowCallback fn_800A4C94(void);
extern u32 __DVDDequeueWaitingQueue(u8* node);
extern void cbForStateMotorStopped_800A65A0(u32 interrupt);
extern void stateReady_800A6684(void);

BOOL DVDCancelAsync(DVDCommandBlock* block, DVDCBCallback callback)
{
    BOOL enabled;
    DVDLowCallback old;

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
            fn_800A4C80();
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
        old = fn_800A4C94();
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
