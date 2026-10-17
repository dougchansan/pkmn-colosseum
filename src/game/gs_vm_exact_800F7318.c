/**
 * @file gs_vm_exact_800F7318.c
 * @brief GS script VM: start a script on its own cooperative thread.
 *
 * Address range: 0x800F7318 - 0x800F7434 (_vmThreadCreate).
 *
 * The thread-creation failure message (lbl_80271294, GS VM string pool)
 * and the name it prints (lbl_80315668) stay extern: this unit is .text
 * only.
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definitions.
 */

#include "dolphin/types.h"
#include "crt/stdarg.h"
#include "game/gs_thread.h"

extern void GSlogWrite(const void* fmt, ...);
extern u32 floorGetCurrentContext(void);
extern void GSthreadSetArgs(void* thread, s32 count, ...);
extern u8* cms_ScriptCreateThread(u32 script, u32 argc, va_list args);
extern u32 cms_ScriptExecute(void* entry);

extern u8 lbl_80271294[];                /* "[%s] failed to create thread" (SJIS) */
extern u8 lbl_80315668[];                /* "_vmThreadCreate", this function's name */

/* 0x800F7318 | 0x11C */
/*
 * Allocates a script task for `script` (fn_800F6D18 pushes argc variadic
 * arguments onto its stack), then runs it on a new GS thread whose entry is
 * the script executor fn_800F6BC4. `done` is called with the task and its
 * result when the script ends. Returns the task key, or 0.
 */
u16 _vmThreadCreate(u32 affinity, u32 script, u32 stackSize, u32 autoStart,
                void* done, u32 argc, ...)
{
    va_list args;
    u8* entry;
    u32 priority;
    GSThread* thread;

    va_start(args, argc);
    entry = cms_ScriptCreateThread(script, argc, args);
    priority = floorGetCurrentContext();
    if (entry == NULL) {
        return 0;
    }
    thread = GSthreadCreate(affinity, priority, stackSize, 1, autoStart, cms_ScriptExecute);
    if (thread != NULL) {
        *(GSThread**)(entry + 0xC) = thread;
        *(void**)(entry + 0x10) = done;
        GSthreadSetArgs(thread, 1, entry);
    } else {
        GSlogWrite(lbl_80271294, lbl_80315668);
    }
    return *(u16*)(entry + 6);
}
