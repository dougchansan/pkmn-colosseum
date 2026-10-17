/**
 * @file fsys_file_candidate_8017C894.c
 * @brief FSYS status-401 handler (0x8017C894 - 0x8017C8C0).
 *
 * Entry of the slot status dispatch table lbl_8036C3E0 (status 401):
 * finish the slot through fsysSlotFinishA and report the frame done.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): the slot parameter is used
 * once, so retail homes it on the stack (stw r3,0x8(r1)) and reloads it.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern void fsysSlotFinishA(FSYSSlot* slot);

/* Address: 0x8017C894 | size: 0x2C */
s32 fsysStatus401(FSYSSlot* slot)
{
    fsysSlotFinishA(slot);
    return 0;
}
