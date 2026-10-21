/**
 * @file gs_vm_exact_800F7068.c
 * @brief GS script VM: script-context queries by key.
 *
 * Address range: 0x800F7068 - 0x800F716C (GSvmIsRunning, GSvmGetThreadByKey).
 *
 * A .text-only piece of the GS VM translation unit (game/gs_vm.c holds the
 * same definitions). Neither function reads the string pool, so the unit is exact on
 * its own with the manager pointer extern.
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"
#include "game/gs_vm.h"

extern void _threadSwitch(void);

/* 0x800F7068 | 0xA0: is the script with `key` still running?  With `wait`,
 * yields until it is not. */
s32 GSvmIsRunning(u16 key, u8 wait)
{
    for (;;) {
        if (GSvmFindByKey(key) == NULL) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

/* 0x800F7108 | 0x64: the thread running the script with `key`. */
GSThread* GSvmGetThreadByKey(u16 key)
{
    GSVMCtx* ctx;

    ctx = GSvmFindByKey(key);
    if (ctx == NULL) {
        return NULL;
    }
    return ctx->thread;
}
