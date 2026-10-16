/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_candidate_80038990.c
 * @brief Byte-exact PDA load-progress thread, 0x80038990 - 0x80038A00.
 */
#include "dolphin/types.h"

extern f32 lbl_8047A494;

void fn_80038990(void)
{
    extern s32 fn_800D37CC(void);
    extern u32 GSgetFrameStep(void);
    extern void _threadSwitch(void);

    while (1) {
        lbl_8047A494 = (f32)GSgetFrameStep() / (f32)fn_800D37CC();
        _threadSwitch();
    }
}
