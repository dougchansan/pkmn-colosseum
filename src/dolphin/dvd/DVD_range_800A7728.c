#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"

extern s32 FatalErrorFlag_8047A800;
extern volatile s32 PausingFlag_8047A7F8;
extern DVDCommandBlock* executing_8047A7E8;
extern DVDCommandBlock DummyCommandBlock_803FC3A0;

static inline s32 dvdGetCommandBlockStatus(DVDCommandBlock* block)
{
    BOOL enabled;
    s32 result;

    enabled = OSDisableInterrupts();
    if (block->state == 3) {
        result = 1;
    } else {
        result = block->state;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 DVDGetCommandBlockStatus(DVDCommandBlock* block)
{
    BOOL enabled;
    s32 result;

    enabled = OSDisableInterrupts();
    if (block->state == 3) {
        result = 1;
    } else {
        result = block->state;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 DVDGetDriveStatus(void)
{
    BOOL enabled;
    s32 result;

    enabled = OSDisableInterrupts();

    if ((s32) FatalErrorFlag_8047A800 != 0) {
        result = -1;
    } else if ((s32) PausingFlag_8047A7F8 != 0) {
        result = 8;
    } else if (executing_8047A7E8 == NULL) {
        result = 0;
    } else if (executing_8047A7E8 == &DummyCommandBlock_803FC3A0) {
        result = 0;
    } else {
        result = dvdGetCommandBlockStatus(executing_8047A7E8);
    }

    OSRestoreInterrupts(enabled);
    return result;
}
