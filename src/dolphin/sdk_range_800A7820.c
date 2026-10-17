/**
 * @file sdk_range_800A7820.c
 * @brief dolphin-sdk code, 0x800A7820 - 0x800A7AFC (3 fns).
 *
 * DVDCancelAsync remains in the extracted object until strict byte-exact
 * source is proven; the range name preserves address traceability.
 */
#include "dolphin/types.h"
#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"

s32 DVDSetAutoInvalidation(s32 arg0) {
    extern s32 autoInvalidation_804789CC;
    s32 oldValue = autoInvalidation_804789CC;

    autoInvalidation_804789CC = arg0;
    return oldValue;
}

void DVDResume(void) {
    extern volatile s32 PauseFlag_8047A7F4;
    extern volatile s32 PausingFlag_8047A7F8;
    extern void stateReady_800A6684(void);
    BOOL enabled = OSDisableInterrupts();

    PauseFlag_8047A7F4 = FALSE;
    if (PausingFlag_8047A7F8 != FALSE) {
        PausingFlag_8047A7F8 = FALSE;
        stateReady_800A6684();
    }
    OSRestoreInterrupts(enabled);
}
