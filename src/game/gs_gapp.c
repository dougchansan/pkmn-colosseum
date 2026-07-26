/**
 * @file gs_gapp.c
 * @brief GSgapp -- GSAPI "gapp" cooperative task wrapper (block/unblock,
 *        terminate, update, create/init) built atop the GSthread task
 *        list.
 *
 * Address range: 0x800FE6DC - 0x800FEC34 (8 functions per the r3 spec's
 * symbols.txt containment mapping; gappVSyncCallback (0x800FEBA0) is
 * nominally within this range but is not physically defined anywhere in
 * the original gs_thread_hi.c source -- it is only forward-declared
 * here and is actually defined in game/gapp_range_80101B34.c. This is a
 * pre-existing cross-file placement issue predating this split and is
 * out of scope for this change; only the 7 functions physically present
 * (GSgappUnblock, GSgappBlock, GSgappTerminate, GSgappUpdate,
 * GSgappCreate, GSgappInit, gappBackgroundCallback) are moved here.
 * XD class: game/pxdvs/GSAPI/GSgapp/GSgapp.cpp
 *
 * Split out of the former monolithic game/gs_thread_hi.c
 * (0x800F8268-0x800FF0A0 per config/GC6E01/splits.txt).
 */
#include "dolphin/types.h"
#include "game/gs_thread.h"


/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const void* fmt, ...);          /* OSReport */
extern u16   GSmemAllocRaw(u32 size);                    /* _toolentryAlloc__FUl */
extern void* GSmemGetPtr(u16 handle);                    /* fn_800E27B0 */
extern void* GSmemLock(u16 handle);                      /* fn_800E24B0 */
extern void  GSmemFree(u16 handle);                      /* fn_800E209C */
extern u16   GSmemAlloc(u32 alignment, u32 size);        /* fn_800E2C04 */
extern void  OSSetIdleFunction(void* func, void* arg,
                          void* stackTop, u32 stackSize); /* OSCreateFiber-like */
extern void  OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(void);
extern void  fn_800D30A0(void* callback);                 /* GSgfx register swap callback */
extern void  threadSaveGPRRegisters(void);                /* GSthread context init */
extern void  threadSaveFPRRegisters(void);                           /* GSthread FPU context init */
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GSscratchFree(void*);
extern void cos();   /* MSL trig (renamed fn_800CDBE0) - referenced by asm incs */

/* ===== String constants (rodata references) ===== */
extern const char lbl_80271008[]; /* "GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n" */

/* ===== Forward declarations for internal functions ===== */
extern void gappVSyncCallback(void);            /* GStaskSwapCallback */
extern void fn_800F0F4C(u32 arg);          /* GSthread trampoline / entry wrapper */
extern void fn_800AB150(void* buf);
extern u32 fn_800D0F44(u32 buttonIdx);
extern void fn_800AB4FC(void*);
extern void fn_800E209C(u16 handle);
extern void* fn_800E24B0(u16 handle);
extern void* fn_800E27B0(u16 handle);
extern u16 fn_800E2C04(u32 alignment, u32 size);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void fn_80080ED8(void);
extern void fn_800DBEB4(u32 a, void* b);
extern void fn_800D5CB8(s32 a, s32 b, s32 c, s32 d, s32 e);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D6728(void);
extern void fn_800D67BC(s32 a);
extern void fn_800D6A00(s32 a);
extern void fn_800D7820(void* ptr);
extern void fn_800D85D4(void);
extern void fn_800D888C(u32 mask, ...);
extern void fn_800D88DC(u32 mask);
extern void fn_800D9ED8(void);
extern void fn_800DC1D4(s32 a);
extern void logVsnprintf_float(void);
extern void GStextureUnlockImage(void* ctx);
extern void GStextureLockImage(void);
extern void fn_801669BC(u32 type);
extern void* GStextureCreate(u16 width, u16 height, u32 format, u32 tlutFormat, u8 mipLevels);
extern void fn_800CDBE0(void);
extern u32 fn_800D3088(void);
extern void fn_800DBF78(void);
extern void fn_800DBFD4(void);
extern void fn_800DC04C(void);
extern void fn_800DC0D4(void);
extern void fn_800DC14C(void);
extern void fn_800DC224(void);
extern void windowDrawSprite(void);
extern void fn_80166A28(void);
extern void fn_800D59B8(void);
extern void fn_800D5BA0(void);
extern void fn_800D9D68(u16 a, u16 b, u16 c, u16 d);
extern f64 tan(void);
extern void fn_800D7FE4(void* mtx);
extern void fn_800D834C(void);
extern void fn_800D9BD0(f32 a, f32 b, f32 c, f32 d);
extern void fn_800DA028(s32 a);
extern void fn_800DA100(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void fn_800DA1E8(s32 a, s32 b, s32 c);
extern void fn_800DA2BC(s32 a, s32 b, s32 c);
extern void fn_800DA4C4(s32 a, s32 b, s32 c);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E0218(void* dst, void* a, void* b, void* c);
extern void* memset(void* dest, int val, u32 n);
extern void* memcpy(void* dst, const void* src, u32 n);

/* ===== BSS/SDA symbol externs (for asm{} blocks) ===== */
/* BSS/data/rodata symbols accessed via lis/@ha + addi/@l pairs */
extern u32 lbl_80401C10;
/* .bss symbols */
extern u8  lbl_80401DE0[];
extern u8  lbl_80401E48[];
extern u8  lbl_80402418[];
extern u8  lbl_80402480[];
extern u8  lbl_804024E8[];
/* .data symbols */
extern u8  lbl_80314E08[];
extern u8  lbl_80314F98[];
extern u8  lbl_80315678[];
/* .rodata symbols */
extern u8  lbl_80271300[];
extern u8  lbl_80271500[];
extern u8  lbl_80271700[];
extern u8  lbl_80271730[];
extern u8  lbl_80271754[];
extern u8  lbl_8027177C[];
extern u8  lbl_802717B4[];
extern u8  lbl_802717D4[];
/* .sdata symbol */
extern float lbl_80478AC0;
/* sdata2 (r2) float/double constants used in asm blocks */
extern f64 lbl_8047CCC8;  /* f64 */
extern f32 lbl_8047CCD0;  /* f32 */
extern f32 lbl_8047CCD4;  /* f32 */
extern f32 lbl_8047CCD8;  /* f32 */
extern f32 lbl_8047CCDC;  /* f32 */
extern f64 lbl_8047CCE0;  /* f64 */
extern f64 lbl_8047CCE8;  /* f64 */
extern f64 lbl_8047CCF0;  /* f64 */
extern f64 lbl_8047CCF8;  /* f64 */
extern u32 lbl_8047CD00;  /* u32 (lwz) */
extern u32 lbl_8047CD04;  /* u32 (lwz) */
extern f32 lbl_8047CD08;  /* f32 */
extern f64 lbl_8047CD10;  /* f64 */
extern f64 lbl_8047CD18;  /* f64 */
extern f64 lbl_8047CD20;  /* f64 */
extern f64 lbl_8047CD28;  /* f64 */
extern f32 lbl_8047CD30;  /* f32 */
extern f32 lbl_8047CD34;  /* f32 */
extern f32 lbl_8047CD38;  /* f32 */
extern f32 lbl_8047CD3C;  /* f32 */
extern f32 lbl_8047CD40;  /* f32 */
extern f32 lbl_8047CD44;  /* f32 */
extern f32 lbl_8047CD48;  /* f32 */
extern f32 lbl_8047CD4C;  /* f32 */
extern f64 lbl_8047CD50;  /* f64 */
extern f32 lbl_8047CD58;  /* f32 */
extern f32 lbl_8047CD5C;  /* f32 */
extern f32 lbl_8047CD60;  /* f32 */
extern f32 lbl_8047CD64;  /* f32 */
extern f32 lbl_8047CD68;  /* f32 */
extern f32 lbl_8047CD6C;  /* f32 */
extern f32 lbl_8047CD70;  /* f32 */
extern f32 lbl_8047CD74;  /* f32 */
extern f32 lbl_8047CD78;  /* f32 */
/* sbss (r13) symbols -- task and thread system */
extern u32 lbl_80478B08;
extern u32 lbl_80478B10;
extern u32 lbl_80478B14;
extern u32 lbl_8047AC00;
extern u32 lbl_8047AC04;
extern u32 lbl_8047AC08;
extern u32 lbl_8047AC0C;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC24;
extern u32 lbl_8047AC28;
extern u32 lbl_8047AC2C;
extern u32 lbl_8047AC30;
extern u32 lbl_8047AC34;
extern u32 lbl_8047AC38;
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;
extern u32 lbl_8047AC44;
extern u32 lbl_8047AC48;
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
extern u32 lbl_8047AC54;
extern u16 lbl_8047AC58;
extern u32 lbl_8047AC5C;
extern u32 lbl_8047AC60;
extern u32 lbl_8047AC64;
extern u32 lbl_8047AC68;
extern u32 lbl_8047AC6C;
extern u32 lbl_8047AC70;
extern u32 lbl_8047AC72;
extern u32 lbl_8047AC74;
extern u16 lbl_8047AC78;
extern u32 lbl_8047AC7C;
extern u32 lbl_8047AC80;
extern u32 lbl_8047AC84;
extern u32 lbl_8047AC88;
extern u16 lbl_8047AC8C;
extern u32 lbl_8047AC90;
extern u32 lbl_8047AC94;
extern u32 lbl_8047AC98;
extern u32 lbl_8047AC9C;

/* Forward declarations for all asm-wrapped functions in this block */
extern void fn_800F8268(void);
extern void fn_800F8428();
extern void fn_800F8654();
extern void fn_800F8A54();
extern u32 fn_800F92D4(u32 key);
extern void GSresInit(u32 count);
extern u8 * fn_800F96E4();
extern u32 fn_800F9AEC(void* outbuf, u16* src, s32 mode);
extern void GScharMakeFromSJIS(void);
extern u8* GScharCpy(u8* dst, u8* src);
extern void GSmsgSetColor(void* obj);
extern s32 GSmsgGetRect();
extern void GSmsgInitRuby();
extern s32 fn_800FAEF8();
extern s32 fn_800FB43C();
extern s32 fn_800FB680();
extern s32 fn_800FB8C8();
extern s32 fn_800FBB34();
extern void GSmsgDaemon(void);
extern s32 GSmsgExec();
extern void fn_800FC2A4(void);
extern u32 fn_800FC2A8(void* ptr);
extern void* GSmsgFontOpen();
extern s32 GSmsgSetCtrlFunc(u32 val);
extern s32 GSmsgInit();
extern s32 fn_800FC7E0();
extern void fn_800FD348();
extern void fn_800FD69C();
extern u16 * _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO();
extern s32 _msgGetLength__FPCUs(const void* str);
extern s32 _msgGetSize__FPCUs();
extern void fn_800FE35C(void);
extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
extern void spriteSetEnv(void);
extern void fn_800FE6A0(f32 a, f32 b);
extern void fn_800FE6AC(s16* outA, s16* outB);
extern void fn_800FE6D0(s32 a, s32 b);
extern void GSgappUnblock(u32 taskId);
extern void GSgappBlock(u32 taskId);
extern void GSgappTerminate(u32 taskId);
extern void GSgappUpdate(void);
extern u32 GSgappCreate(s32 state, u8 priority, void* param, void* func);
extern void GSgappInit();
extern void gappBackgroundCallback(void);

/* 0x800FE6DC | 0x1C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void GSgappUnblock(void) {
#include "src/game/gs_thread_GSgappUnblock.inc"
}
#else
#pragma optimization_level 2
void GSgappUnblock(u32 taskId) {
    u32 idx = taskId - 1;
    ((u8*)lbl_8047AC7C)[idx * 0x18 + 0xD] = 0;
}
#endif
#pragma pop

/* 0x800FE6F8 | 0x1C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void GSgappBlock(void) {
#include "src/game/gs_thread_GSgappBlock.inc"
}
#else
#pragma optimization_level 2
void GSgappBlock(u32 taskId) {
    u32 idx = taskId - 1;
    ((u8*)lbl_8047AC7C)[idx * 0x18 + 0xD] = 1;
}
#endif
#pragma pop

/* 0x800FE714 | 0x8C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void GSgappTerminate(s32 taskId) {
#include "src/game/gs_thread_fn_800FE714.inc"
}
#else
void GSgappTerminate(u32 taskId) {
    extern u32 lbl_8047AC7C;
    extern u32 lbl_8047AC98;
    extern u32 OSDisableInterrupts(void);
    extern void OSRestoreInterrupts(u32 state);
    u8* node;
    u32 state;
    u32 prev;
    u32 next;
    node = (u8*)lbl_8047AC7C + (taskId - 1) * 0x18;
    state = OSDisableInterrupts();
    prev = *(u32*)(node + 0);
    if (prev != 0) { *(u32*)(prev + 4) = *(u32*)(node + 4); }
    next = *(u32*)(node + 4);
    if (next != 0) { *(u32*)(next + 0) = *(u32*)(node + 0); }
    if (lbl_8047AC98 == (u32)node) { lbl_8047AC98 = *(u32*)(node + 4); }
    *(u32*)(node + 0) = 0;
    *(u32*)(node + 4) = 0;
    OSRestoreInterrupts(state);
    *(u32*)(node + 8) = 0;
}
#endif
#pragma pop

/* 0x800FE7A0 | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void GSgappUpdate(void) {
#include "src/game/gs_thread_GSgappUpdate.inc"
}
#else
#pragma optimization_level 2
void GSgappUpdate(void) {
    u32* task;
    u32* next;
    u32 taskId;
    task = (u32*)lbl_8047AC98;
    while (task != NULL) {
        next = (u32*)task[1];
        if ((s32)task[2] == 1 && ((u8*)task)[0xD] == 0) {
            lbl_8047AC94 = (u32)task;
            taskId = ((u32)task - lbl_8047AC7C) / 0x18 + 1;
            ((void (*)(u32, void*))task[5])(taskId, (void*)task[4]);
        }
        task = next;
    }
    lbl_8047AC94 = 0;
}
#endif
#pragma pop

/* 0x800FE834 | 0x17C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm u32 GSgappCreate(s32 state, u8 priority, void* param, void* func) {
#include "src/game/gs_thread_GSgappCreate.inc"
}
#else
#pragma optimization_level 2
#pragma scheduling on
u32 GSgappCreate(s32 state, u8 priority, void* param, void* func) {
    u8* task;
    u32 count;
    u32 i;
    u8* curr;
    u8* prev;
    u8* next;

    if (state == 2) {
        task = (u8*)lbl_8047AC7C + lbl_8047AC80 * 0x18;
        count = lbl_8047AC84;
    } else {
        task = (u8*)lbl_8047AC7C;
        count = lbl_8047AC80;
    }
    i = count;
    if (i != 0) {
        do {
            if (*(s32*)(task + 0x8) == 0) goto found;
            task += 0x18;
            i--;
        } while (i != 0);
    }
    task = NULL;
found:
    if (task == NULL) {
        return 0;
    }
    *(u32*)(task + 0x0) = 0;
    *(u32*)(task + 0x4) = 0;
    *(u32*)(task + 0x8) = state;
    *(u8*)(task + 0xC) = priority;
    *(u8*)(task + 0xD) = 0;
    *(u32*)(task + 0x10) = (u32)param;
    *(u32*)(task + 0x14) = (u32)func;
    if (lbl_8047AC98 == 0) {
        lbl_8047AC98 = (u32)task;
    } else {
        OSDisableInterrupts();
        if (*(s32*)(task + 0x8) == 2) {
            *(u32*)(task + 0x4) = lbl_8047AC9C;
            lbl_8047AC9C = (u32)task;
        } else {
            curr = (u8*)lbl_8047AC98;
            for (;;) {
                next = (u8*)*(u32*)(curr + 0x4);
                if (next == NULL) break;
                if ((u32)*(u8*)(curr + 0xC) >= (u32)*(u8*)(task + 0xC)) break;
                curr = next;
            }
            if (next == NULL && (u32)*(u8*)(curr + 0xC) < (u32)*(u8*)(task + 0xC)) {
                *(u32*)(task + 0x0) = (u32)curr;
                *(u32*)(task + 0x4) = 0;
                *(u32*)(curr + 0x4) = (u32)task;
            } else {
                prev = (u8*)*(u32*)(curr + 0x0);
                if (prev != NULL) *(u32*)(prev + 0x4) = (u32)task;
                *(u32*)(task + 0x0) = *(u32*)(curr + 0x0);
                *(u32*)(task + 0x4) = (u32)curr;
                *(u32*)(curr + 0x0) = (u32)task;
                if (lbl_8047AC98 == (u32)curr) lbl_8047AC98 = (u32)task;
            }
        }
        OSRestoreInterrupts();
    }
    return ((u32)task - lbl_8047AC7C) / 0x18 + 1;
}
#endif
#pragma pop

/* 0x800FE9B0 | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void GSgappInit(numTasks, numQueues)
    u32 numTasks;
    u32 numQueues;
{
#include "src/game/gs_thread_GSgappInit.inc"
}
#else
#pragma optimization_level 2
void GSgappInit(numTasks, numQueues)
    u32 numTasks;
    u32 numQueues;
{
    u32 total;
    u32 i;
    u32 offset;
    u16 handle;

    total = numTasks + numQueues;
    lbl_8047AC80 = numTasks;
    lbl_8047AC84 = numQueues;
    lbl_8047AC88 = total;
    lbl_8047AC94 = 0;
    handle = GSmemAllocRaw(total * 0x18);
    lbl_8047AC78 = handle;
    if ((u16)handle == 0) return;
    lbl_8047AC7C = (u32)GSmemGetPtr(handle);
    offset = i = 0;
    while (i < lbl_8047AC88) {
        *(u32*)(lbl_8047AC7C + offset + 8) = 0;
        offset += 0x18;
        i++;
    }
    handle = GSmemAllocRaw(0x2000);
    lbl_8047AC8C = handle;
    lbl_8047AC90 = (u32)GSmemGetPtr(handle);
    OSSetIdleFunction((void*)gappBackgroundCallback, NULL,
                (void*)(lbl_8047AC90 + 0x1FFC), 0x1FFC);
    fn_800D30A0((void*)gappVSyncCallback);
}
#endif
#pragma pop

/* 0x800FEA74 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void gappBackgroundCallback(void) {
#include "src/game/gs_thread_gappBackgroundCallback.inc"
}
#else
#pragma optimization_level 2
static void gappBackgroundCallback(void) {
    u32* task;
    u32* next;
    u32* pend;
    u32* next_pend;
    u8* curr;
    u8* prev;
    u32 taskId;

    for (;;) {
        task = (u32*)lbl_8047AC98;
        while (task != NULL) {
            next = (u32*)task[1];
            if (task[2] == 2 && ((u8*)task)[0xD] == 0) {
                lbl_8047AC94 = (u32)task;
                taskId = ((u32)task - lbl_8047AC7C) / 0x18 + 1;
                ((void(*)(u32, void*))task[5])(taskId, (void*)task[4]);
            }
            task = next;
        }
        lbl_8047AC94 = 0;
        OSDisableInterrupts();
        pend = (u32*)lbl_8047AC9C;
        while (pend != NULL) {
            next_pend = (u32*)pend[1];
            curr = (u8*)lbl_8047AC98;
            while (*(u32*)(curr + 0x4) != 0) {
                if (*(u8*)(curr + 0xC) >= *(u8*)((u8*)pend + 0xC)) break;
                curr = (u8*)*(u32*)(curr + 0x4);
            }
            if (*(u32*)(curr + 0x4) == 0 && *(u8*)(curr + 0xC) < *(u8*)((u8*)pend + 0xC)) {
                *(u32*)((u8*)pend + 0x0) = (u32)curr;
                *(u32*)((u8*)pend + 0x4) = 0;
                *(u32*)(curr + 0x4) = (u32)pend;
            } else {
                prev = (u8*)*(u32*)(curr + 0x0);
                if (prev != NULL) *(u32*)(prev + 0x4) = (u32)pend;
                *(u32*)((u8*)pend + 0x0) = *(u32*)(curr + 0x0);
                *(u32*)((u8*)pend + 0x4) = (u32)curr;
                *(u32*)(curr + 0x0) = (u32)pend;
                if (lbl_8047AC98 == (u32)curr) lbl_8047AC98 = (u32)pend;
            }
            pend = next_pend;
        }
        lbl_8047AC9C = 0;
        OSRestoreInterrupts();
    }
}
#endif
#pragma pop

typedef void (*GSgappCallback)(u32 taskId, void* param);

typedef struct GSgappTask {
    struct GSgappTask* prev;
    struct GSgappTask* next;
    s32 state;
    u8 priority;
    u8 blocked;
    u8 _pad[2];
    void* param;
    GSgappCallback callback;
} GSgappTask;

/* 0x800FEBA0 | 0x94 */
void gappVSyncCallback(void) {
    GSgappTask* task;
    GSgappTask* next;
    u32 taskId;

    task = (GSgappTask*)lbl_8047AC98;
    while (task != NULL) {
        next = task->next;
        if (task->state == 3 && task->blocked == 0) {
            lbl_8047AC94 = (u32)task;
            taskId = ((u32)task - lbl_8047AC7C) / sizeof(GSgappTask) + 1;
            task->callback(taskId, task->param);
        }
        task = next;
    }
    lbl_8047AC94 = 0;
}
