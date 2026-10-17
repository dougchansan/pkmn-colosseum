/**
 * @file fsys_file_candidate_8017C008.c
 * @brief FSYS status-103 handler (0x8017C008 - 0x8017C074).
 *
 * Entry of the slot status dispatch table lbl_8036C3E0 (status 103): when
 * the current entry's buffer holds an "LZSS" image, hand it to
 * fn_80180C78, then move the slot to status 100.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

#define FSYS_LZSS_MAGIC 0x4C5A5353 /* 'LZSS' */

extern void fn_80180C78(FSYSSlot* slot, FSYSSubEntry* sub, u32 arg);

/* Address: 0x8017C008 | size: 0x6C */
s32 fsysAllocLZSSJob(FSYSSlot* slot)
{
    FSYSSubEntry* sub;
    u32* data;

    sub = slot->currentSub;
    data = sub->buffer;
    if (*data == FSYS_LZSS_MAGIC) {
        fn_80180C78(slot, sub, 0);
    }
    slot->status = 100;
    return 0;
}
