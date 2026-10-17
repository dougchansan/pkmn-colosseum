/**
 * @file fsys_file_r56_8017F108_suffix.c
 * @brief FSYS DVD/ARQ completion callbacks (0x8017F108 - 0x8017F2C4).
 *
 * fsysDVDReadCallback is the DVD read completion callback the fsys loaders pass
 * (fsysStartLoadMode1.., fsysSlotReadNextEntry, the entry readers after 0x8017B4BC): under
 * disabled interrupts it moves the active slot to the status that follows
 * the one it was waiting in (or marks a failed read with archiveHandle -1)
 * and closes the slot's open file. fsysARQCallback is the ARQ completion
 * callback fsysLoadEntry and fsysSlotStatusStartEntries pass to fn_80180584 (userData is
 * the slot): status 162 on success, 152 otherwise.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): both functions are exact with
 * that single unit-wide flag and no local pragmas; single-use parameters
 * are homed on the stack (stw r3,0x8(r1) / stw r4,0xc(r1)).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern void GSdvdClose(void* file);

/* Address: 0x8017F108 | size: 0x154 */
void fsysDVDReadCallback(s32 result)
{
    FSYSSlot* slot;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    slot = lbl_80453FEC.activeSlot;

    if (result == -1) {
        slot->archiveHandle = -1;
    } else {
        switch (slot->status) {
        case 1:
            slot->status = 2;
            break;
        case 3:
            slot->status = 4;
            break;
        case 101:
            slot->status = 150;
            break;
        case 200:
            slot->status = 201;
            break;
        case 303:
            slot->status = 304;
            break;
        case 400:
            slot->status = 401;
            break;
        case 2:
        case 4:
        case 100:
        case 201:
        case 301:
            break;
        default:
            slot->status = 1000;
            slot->archiveHandle = 1;
            break;
        }
        if (slot->tocBuffer) {
            GSdvdClose(slot->tocBuffer);
            slot->tocBuffer = NULL;
        }
    }
    OSRestoreInterrupts(enabled);
}

/* Address: 0x8017F25C | size: 0x68 */
void fsysARQCallback(s32 result, void* userData)
{
    FSYSSlot* slot;
    BOOL enabled;

    slot = userData;
    enabled = OSDisableInterrupts();
    if (result == 1) {
        slot->status = 0xA2;
    } else {
        slot->status = 0x98;
    }
    OSRestoreInterrupts(enabled);
}
