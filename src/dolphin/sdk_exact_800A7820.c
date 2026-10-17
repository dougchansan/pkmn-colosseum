/* Canonical Dolphin DVD invalidation and resume helpers. */
#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"

extern s32 autoInvalidation_804789CC;
extern volatile s32 PauseFlag_8047A7F4;
extern volatile s32 PausingFlag_8047A7F8;
extern void stateReady_800A6684(void);

s32 DVDSetAutoInvalidation(s32 arg0)
{
    s32 oldValue = autoInvalidation_804789CC;

    autoInvalidation_804789CC = arg0;
    return oldValue;
}

void DVDResume(void)
{
    BOOL enabled = OSDisableInterrupts();

    PauseFlag_8047A7F4 = FALSE;
    if (PausingFlag_8047A7F8 != FALSE) {
        PausingFlag_8047A7F8 = FALSE;
        stateReady_800A6684();
    }
    OSRestoreInterrupts(enabled);
}
