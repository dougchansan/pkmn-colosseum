/**
 * @file fsys_file_candidate_8017D3D4.c
 * @brief FSYS status-2 handler (0x8017D3D4 - 0x8017D400).
 *
 * Entry of the slot status dispatch table lbl_8036C3E0 (status 2): run
 * fsysSlotReadNextEntry on the slot and report the frame done.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): the slot parameter is used
 * once, so retail homes it on the stack (stw r3,0x8(r1)) and reloads it.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern void fsysSlotReadNextEntry(FSYSSlot* slot);

/* Address: 0x8017D3D4 | size: 0x2C */
s32 fsysStatus2(FSYSSlot* slot)
{
    fsysSlotReadNextEntry(slot);
    return 0;
}
