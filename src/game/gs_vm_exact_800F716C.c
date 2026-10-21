/**
 * @file gs_vm_exact_800F716C.c
 * @brief GS script VM: stop the scripts of a thread group.
 *
 * Address range: 0x800F716C - 0x800F7274 (GSvmStopGroup).
 *
 * A .text-only piece of the GS VM translation unit (game/gs_vm.c holds the
 * same definition). It does not read the string pool, so the unit is exact
 * on its own with the manager pointer extern.
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"
#include "game/gs_vm.h"

extern u32 fn_800F0374(GSThread* thread);

/* 0x800F716C | 0x108: stop every script running on a thread of `group`
 * (the thread's +0x0C field, read by fn_800F0374; floor teardown passes
 * the floor id).
 * The thread's group is read into its own variable before the compare:
 * retail compares "cmplw r3,r30" (the group read, then the argument).
 * MWCC (GC/1.3 through 2.7, -O4,p/-O4,s/-O3/-O2) turns an integer equality
 * with a call on either side, fn_800F0374(t) == group or
 * group == fn_800F0374(t), into "cmplw r30,r3"; the call-first order only
 * comes out when the call's value is a named variable (or when both sides
 * are pointers, as in fight_target.c's (void*)fightTrainerGetStatus(...)
 * == (void*)target, which is not the case for a floor id). */
s32 GSvmStopGroup(u32 group)
{
    GSVMCtx* ctx;
    u32 threadGroup;
    s32 i;

    for (i = 0; i < lbl_80478B00->count; i++) {
        ctx = &lbl_80478B00->contexts[i];
        if (ctx->status != 0 && ctx->thread != NULL) {
            threadGroup = fn_800F0374(ctx->thread);
            if (threadGroup == group) {
                GSvmStopByKey(ctx->key);
            }
        }
    }
    return 0;
}
