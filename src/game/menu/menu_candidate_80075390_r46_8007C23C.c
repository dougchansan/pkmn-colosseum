/**
 * @file menu_candidate_80075390_r46_8007C23C.c
 * @brief _gbaBootAlarm (0x8007C23C - 0x8007C260): the GBA boot delay's alarm
 *        handler. It resumes the thread stored 0x28 bytes into the boot
 *        context, whose first member is the alarm (see GBA_BOOT_DELAY in
 *        menu_range_8007109C.c).
 *
 * Function-boundary carve, text only, GC/1.3 -O4,p with the peephole pass
 * off for the whole unit, as the menu units around it (the range file wraps
 * it in `#pragma scheduling off`; either setting puts the stw r0 before the
 * addi as retail does, and the menu TU's evidence is for the peephole flag).
 */
#include "dolphin/types.h"

extern s32 OSResumeThread(void* thread);

void _gbaBootAlarm(u8* context)
{
    OSResumeThread(context + 0x28);
}
