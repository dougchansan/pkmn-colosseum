/**
 * @file gs_vm_exact_800F13D0.c
 * @brief GS script VM native "wait" call.
 *
 * Address range: 0x800F13D0 - 0x800F16C0 (fn_800F13D0).
 *
 * A .text-only unit carved from the GS VM translation unit (game/gs_vm.c,
 * which holds the same definition). The call-frame entry is the shared
 * GSvmEnterNative helper that fn_800F10E8 and fn_800F16C0 also expand.
 */

#include "dolphin/types.h"
#include "game/gs_vm.h"

extern u32 GSgetFrameStep(void);
extern void _threadSwitch(void);

/* 0x800F13D0 | 0x2F0 */
/*
 * Native "wait" call: yields the script thread (_threadSwitch) until the
 * per-resume deltas from fn_800D3088 add up to argument 0.  The VM state byte
 * (+0x04) is 2 while waiting; an abort request (4 -> 3) leaves early with 0.
 */
s32 cms_op36_yield(GSVMCtx* ctx)
{
    s32 wait;
    s32 elapsed;
    GSVMValue* args;

    GSvmEnterNative(ctx);
    args = &ctx->stack[ctx->frame];
    wait = args[0].s;
    ctx->status = 2;
    for (elapsed = 0; elapsed < wait; elapsed += GSgetFrameStep()) {
        if (ctx->status == 4) {
            ctx->status = 3;
        }
        if (ctx->status == 3) {
            GSvmReturn(ctx);
            return 0;
        }
        _threadSwitch();
    }
    ctx->status = 1;
    GSvmReturn(ctx);
    return 1;
}
