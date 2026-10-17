/**
 * @file fsys_file_r49_8017CE7C_suffix.c
 * @brief FSYS status-2000 handler (0x8017CE7C - 0x8017CEC8).
 *
 * Entry of the slot status dispatch table lbl_8036C3E0 (status 2000):
 * load mode 3 slots are released through fn_8017D960, every other mode
 * through fsysSlotCheckEntries.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern void fn_8017D960(FSYSSlot* slot);
extern void fsysSlotCheckEntries(FSYSSlot* slot);

/* Address: 0x8017CE7C | size: 0x4C */
s32 fsysSlotStatus2000(FSYSSlot* slot)
{
    if ((s32)slot->loadMode == 3) {
        fn_8017D960(slot);
    } else {
        fsysSlotCheckEntries(slot);
    }
    return 1;
}
