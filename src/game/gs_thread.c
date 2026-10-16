/**
 * @file gs_thread.c
 * @brief GSthread -- Genius Sonority cooperative task / thread system.
 *
 * Decompiled from:
 *   GSthreadCreate (GSthreadCreate) -- real, matches config/GC6E01/symbols.txt
 *
 * A prior recovery pass invented GStaskInit, GStaskCreate, GStaskRun,
 * GSthreadInit, and a private GStaskSchedulerThread helper in this file
 * (claiming addresses GSgappInit/GSgappCreate/GSgappUpdate/gappBackgroundCallback),
 * with the exact same fabricated bodies duplicated verbatim into
 * gs_thread_hi.c. None of those four public names appear in symbols.txt,
 * and the claimed addresses all fall in gs_thread_hi.c's real range
 * (0x800F8268-0x800FF0A0), not this file's (0x800F0030-0x800F75FC). They
 * have been removed as dead/fictional code; see gs_thread_hi.c for the
 * real (address-scaffolded, still largely unmatched) definitions of
 * GSgappInit/GSgappCreate/GSgappUpdate/gappBackgroundCallback.
 *
 * This file's range now starts at 0x800F0030 (previously 0x800F07A8):
 * the block 0x800F0030-0x800F07A8 (21 functions -- GSthread cooperative-
 * scheduler primitives: threadLoadGPRRegisters, threadSaveGPRRegisters,
 * threadExecute, _threadSwitch, GSthreadUnblockGroup, GSthreadBlockGroup,
 * GSthreadUnblock/Block/ExecuteAll/ExecuteGroup/Close/IsRunning/
 * TerminateGroup/Terminate/SetArgs, etc.) was moved here from
 * src/game/gs_texture.c, which had mistakenly retained it -- confirmed
 * against the XD reference splits (game/pxdvs/GSAPI/GSthread/GSthread.cpp,
 * 0x8010315C-0x80103F70) to be this same TU's real prefix.
 *
 * This file's only genuinely decompiled/matched function is GSthreadCreate
 * below. Everything else is unrelated stub scaffold
 * for the rest of this unit's .text range.
 *
 * The task system and thread system are two separate but related layers:
 *
 * 1. TASKS (lightweight):
 *    - Array of GSTask structs, each 0x18 (24) bytes.
 *    - Kept in a priority-sorted singly-linked list.
 *    - Used for per-frame work: VBlank, pad polling, audio, reset.
 *    - Not yet decompiled under a real name in this file (see note above).
 *
 * 2. THREADS (heavier cooperative fibres):
 *    - Array of GSThread structs, each 0x24 (36) bytes.
 *    - Own a GSmem-allocated stack and context block.
 *    - Managed in a priority-sorted doubly-linked list.
 *    - Used for the main game loop and long-running subsystems.
 *
 * Address range: 0x800F0030 - 0x800F10E8 (the GS VM unit that follows is
 * game/gs_vm.c and its linked .text-only pieces).
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"

typedef u8 M2C_UNK;
#define M2C_FIELD(base, type, offset) (*(type)((u8*)(base) + (offset)))
#define M2C_BITWISE(type, value) (*(type*)&(value))

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
extern void cos();   /* MSL trig (renamed fn_800CDBE0) -- referenced by asm incs */

/* ===== String constants (rodata references) ===== */
extern const char lbl_80271008[]; /* "GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n" */
extern const char lbl_80271038[]; /* "GSthread: Init OK, maximum of %d threads\n" */

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
extern void gfxEnd(void);
extern void gfxBegin(s32 a);
extern void gfxSetPrimitive(s32 a);
extern void gfxSetVtxDesc(void* ptr);
extern void gfxLoadTexture(void);
extern void gfxClearFlags(u32 mask, ...);
extern void gfxSetFlags(u32 mask);
extern void fn_800D9ED8(void);
extern void fn_800DC1D4(s32 a);
extern void logVsnprintf_float(void);
extern void GStextureUnlockImage(void* ctx);
extern void GStextureLockImage(void);
extern void soundStopSE(u32 type);
extern void* GStextureCreate(u16 width, u16 height, u32 format, u32 tlutFormat, u8 mipLevels);
extern void fn_800CDBE0(void);
extern u32 GSgetFrameStep(void);
extern void fn_800DBF78(void);
extern void fn_800DBFD4(void);
extern void fn_800DC04C(void);
extern void fn_800DC0D4(void);
extern void fn_800DC14C(void);
extern void gfxSetTevStage(void);
extern void windowDrawSprite(void);
extern void soundStartSE(void);
extern void gfxVtxTexCoord(void);
extern void fn_800D5BA0(void);
extern void fn_800D9D68(u16 a, u16 b, u16 c, u16 d);
extern f64 tan(void);
extern void fn_800D7FE4(void* mtx);
extern void fn_800D834C(void);
extern void fn_800D9BD0(f32 a, f32 b, f32 c, f32 d);
extern void gfxSetCullMode(s32 a);
extern void fn_800DA100(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void gfxSetZMode(s32 a, s32 b, s32 c);
extern void gfxSetUpdates(s32 a, s32 b, s32 c);
extern void gfxSetBlendMode(s32 a, s32 b, s32 c);
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
extern u8 lbl_8047AC0C;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC24;
extern GSThread* lbl_8047AC28;
extern u16 lbl_8047AC2C;
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
extern u32 lbl_8047AC78;
extern u32 lbl_8047AC7C;
extern u32 lbl_8047AC80;
extern u32 lbl_8047AC84;
extern u32 lbl_8047AC88;
extern u32 lbl_8047AC8C;
extern u32 lbl_8047AC90;
extern u32 lbl_8047AC94;
extern u32 lbl_8047AC98;
extern u32 lbl_8047AC9C;
/* sdata2 (r2) symbols: CW asm{} does not support sym(r2) syntax.
 * These are handled via numeric offsets in the .inc files.
 * No extern declarations needed. */

/* ===== Global state (sbss) ===== */

/* Thread system globals (see the sbss externs above):
 *   lbl_8047AC30 : maximum thread count
 *   lbl_8047AC28 : thread array
 *   lbl_8047AC08 : head of the affinity-sorted thread list
 *   lbl_8047AC0C : reschedule flag (byte)
 *   lbl_8047AC1C : context block used by the register save/load primitives
 */

/*
 * GSthread context block layout (pointed to by lbl_8047AC1C):
 *   +0x00 : r0           (u32)
 *   +0x04 : saved stack pointer (r1)
 *   +0x08 : r2
 *   +0x0C : r3
 *   +0x10 : r4
 *   ...   : r5-r31 at +0x14 .. +0x7C
 *   +0x80 : saved LR
 *   +0x84 : saved CTR / scratch
 *   +0x88 : f0 (f64, 8 bytes each)
 *   ...   : f1-f31 at +0x90 .. +0x180
 * (GSThreadCtx, declared in game/gs_thread.h.)
 */
/* =======================================================================
 *  NOTE (orphan fiction removed): a prior recovery pass invented
 *  GStaskInit, GStaskCreate, GStaskRun, GSthreadInit, and a private
 *  GStaskSchedulerThread helper here, with bodies duplicated verbatim into
 *  gs_thread_hi.c. None of the four public names appear in
 *  config/GC6E01/symbols.txt, and their claimed addresses (GSgappInit,
 *  GSgappCreate, GSgappUpdate, gappBackgroundCallback) fall outside this file's real
 *  range (0x800F07A8-0x800F75FC per config/GC6E01/splits.txt) -- they
 *  belong to gs_thread_hi.c's range (0x800F8268-0x800FF0A0), which already
 *  contains the real stub scaffold for those addresses. The fictional
 *  definitions, their exclusive "task system" globals, and the unused
 *  GStaskSchedulerThread forward declaration have been removed.
 * ======================================================================= */

/* =======================================================================
 *  0x800F0424 - 0x800F106C: GSthreadUnblock .. fn_800F0F4C (GSthread public
 *  API, GSthreadCreate, GSthread, the fn_800F0A74 dispatcher and the
 *  fn_800F0F4C exit trampoline) live in their own linked unit, in address
 *  order. Included here so every candidate unit built from this file keeps
 *  the same definitions.
 * ======================================================================= */
#include "src/game/gs_thread_exact_800F0424.c"


/* =======================================================================
 * The following block (0x800F0030 - 0x800F07A8, 21 functions) was moved
 * here from src/game/gs_texture.c. It was mistakenly left in that file's
 * TU even though it is GSthread.cpp content (cooperative-scheduler
 * primitives), not GStexture.cpp content -- confirmed against the XD
 * reference splits (game/pxdvs/GSAPI/GSthread/GSthread.cpp,
 * 0x8010315C-0x80103F70) whose start lines up byte-exactly with our
 * GStextureInit (ends 0x800F0030) -> threadLoadGPRRegisters (0x800F0030)
 * boundary. This is the real prefix of this file's existing GSthread.cpp
 * content (GSthreadCreate onward, 0x800F07A8-0x800F75FC below).
 *
 * dont_inline: before this merge these functions lived in a separate TU
 * (gs_texture.c), so mwcc's -inline auto could never fold them into any
 * of this file's later callers (threadExecute/_threadSwitch/etc. were
 * only visible as opaque externs here). Bracketing the whole block keeps
 * every later caller's compiled bytes identical to before the merge.
 * ======================================================================= */
#pragma dont_inline on

/* 0x800F0030 - 0x800F036C: the register save/restore and context-switch
 * primitives are first-party assembly in their own linked unit (evidence:
 * docs/asm_evidence/gs_thread.md). */
#include "src/game/gs_thread_exact_800F0030.c"

/* 0x800F028C | 0x68 */
extern void threadSaveGPRRegisters(void);
extern void threadLoadGPRRegisters(void);
extern void threadLoadFPRRegisters(void);
extern u32 lbl_8047AC24;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC10;


/* 0x800F036C | 0x8 */
/* lbz r3, 0xb(r3) -- load byte at offset 0xb from arg */
u8 fn_800F036C(GSThread* thr) {
    return thr->affinity;
}

/* 0x800F0374 | 0x8 */
/* lwz r3, 0xc(r3) -- load word at offset 0xc from arg
 * (parameter kept as u32 to match this file's other fn_800F0374
 * extern declarations/call sites, which pass a raw GSThread* value
 * stored in a u32 slot rather than a typed pointer) */
u32 fn_800F0374(u32 ctx) {
    GSThread* thr = (GSThread*)ctx;
    return thr->priority;
}

/* 0x800F037C | 0x8 */
extern u32 lbl_8047AC00;
/* lwz r3, lbl_8047AC00(r13) -- return gsThreadFrameCount */
u32 GSthreadGetCurrentThread(void) {
    return lbl_8047AC00;
}

/* 0x800F0384 | 0x50 */
extern GSThread* lbl_8047AC28;
extern u32 lbl_8047AC30;
extern u8 lbl_8047AC0C;
/*
 * Loop over all threads; for each thread whose priority == arg, clear sleeping flag.
 * stride = 0x24, field at +0xc = priority, field at +0xa = sleeping
 * lbl_8047AC28 = gsThreadArray, lbl_8047AC30 = gsThreadMaxCount
 * lbl_8047AC0C = gsThreadActive
 */
void GSthreadUnblockGroup(u32 priority) {
    u32 i;
    GSThread* thr;

    if (priority == 0) {
        return;
    }

    for (i = 0; i < lbl_8047AC30; i++) {
        thr = &lbl_8047AC28[i];
        if (thr->priority == priority) {
            thr->sleeping = 0;
        }
    }

    lbl_8047AC0C = 1;
}

/* 0x800F03D4 | 0x50 */
extern GSThread* lbl_8047AC28;
extern u32 lbl_8047AC30;
extern u8 lbl_8047AC0C;
/*
 * Loop over all threads; for each thread whose priority == arg, set sleeping = 1.
 */
void GSthreadBlockGroup(u32 priority) {
    u32 i;
    GSThread* thr;

    if (priority == 0) {
        return;
    }

    for (i = 0; i < lbl_8047AC30; i++) {
        thr = &lbl_8047AC28[i];
        if (thr->priority == priority) {
            thr->sleeping = 1;
        }
    }

    lbl_8047AC0C = 1;
}
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC24;
#pragma dont_inline reset


/* ===================================================================
 * Generated: 1 pattern-matched + 61 stubs
 * Range: 0x800F8268 - 0x800FEBA0
 * =================================================================== */

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
extern s32 std001_messageFormatLookup();
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
extern s32 msgTaskDraw();
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

/* 0x800F8268 | 0x1C0 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800F8268(void) {
    u8 pad[0x50];
    u8* r31;
    u8* r30;
    u8* r28;
    s32 r29;
    u32 btnState;
    s8 ax;

    fn_800AB150(pad);
    r31 = (u8*)&lbl_80401C10;
    r30 = pad;
    for (r29 = 0; r29 < 4; r29++) {
        s32 target = r29 + 1;
        if (*(s32*)r31 == target) {
            r28 = r31;
        } else {
            r28 = r31 + 0x6C;
            if (*(s32*)(r31 + 0x6C) != target) {
                r28 += 0x6C;
                if (*(s32*)(r28) != target) {
                    r28 += 0x6C;
                    if (*(s32*)(r28) != target) {
                        r28 = NULL;
                    }
                }
            }
        }
        if (r28 == NULL) goto next;
        ax = (s8)r30[0xA];
        if (ax == -1) {
            btnState = fn_800D0F44((u32)r29);
            if (btnState == 8) {
                *(u32*)(r28 + 0xC) = 3;
            } else if (btnState == 0x40) {
                *(u32*)(r28 + 0xC) = 4;
            }
            memset(r28 + 0x18, 0, 0xC);
            lbl_8047AC4C = lbl_8047AC4C | ((u32)0x80000000 >> (u32)r29);
        } else if (ax >= 0 && ax < 1) {
            if (*(u32*)(r28 + 0xC) == 3) {
                btnState = fn_800D0F44((u32)r29);
                if (btnState == 0x00900000) {
                    *(u32*)(r28 + 0x4) = 0;
                } else {
                    *(u32*)(r28 + 0x4) = 2;
                }
                *(u32*)(r28 + 0xC) = 0;
            }
            r30[0x3] = (u8)(-(s8)r30[0x3]);
            r30[0x5] = (u8)(-(s8)r30[0x5]);
            memcpy(r28 + 0x18, r30, 0xC);
            lbl_8047AC4C = lbl_8047AC4C & ~((u32)0x80000000 >> (u32)r29);
        }
next:
        r30 += 0xC;
    }
    fn_800F8428();
    lbl_8047AC48++;
}
#pragma pop

/* 0x800F8428 | 0x22C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void fn_800F8428(void) {
    M2C_UNK *var_r4;
    s32 *var_r6;
    s32 temp_r10;
    s32 temp_r8;
    s32 temp_r8_2;
    s32 temp_r8_3;
    s32 var_ctr;
    s32 var_r7;
    u32 *var_r5;
    u32 temp_r8_4;
    u8 var_r0;

    var_r0 = 0;
    var_r7 = 0;
    var_r5 = (u32 *)((u8 *)&lbl_80401C10 + 0x1C0);
    var_r6 = (s32 *)((u8 *)&lbl_80401C10 + 0x1B0);
    var_ctr = 4;
    do {
        var_r4 = (u8 *)&lbl_80401C10;
        temp_r10 = var_r7 + 1;
        if ((s32) M2C_FIELD((u8 *)&lbl_80401C10, s32 *, 0) == temp_r10) {

        } else {
            var_r4 = (u8 *)&lbl_80401C10 + 0x6C;
            if ((s32) M2C_FIELD((u8 *)&lbl_80401C10, s32 *, 0x6C) == temp_r10) {

            } else {
                temp_r8 = M2C_FIELD(var_r4, s32 *, 0x6C);
                var_r4 += 0x6C;
                if (temp_r8 == temp_r10) {

                } else {
                    temp_r8_2 = M2C_FIELD(var_r4, s32 *, 0x6C);
                    var_r4 += 0x6C;
                    if (temp_r8_2 == temp_r10) {

                    } else {
                        var_r4 = 0;
                    }
                }
            }
        }
        if ((var_r4 != 0) && ((s32) M2C_FIELD(var_r4, s32 *, 0xC) == 0)) {
            temp_r8_3 = M2C_FIELD(var_r4, s32 *, 0x5C);
            switch (temp_r8_3) {                    /* irregular */
            case 1:
                if (((u32) M2C_FIELD(var_r4, u32 *, 0x60) > 0xE10U) && ((u32) *var_r5 != 1U)) {
                    var_r0 = 1;
                    *var_r5 = 1U;
                } else if (((u32) M2C_FIELD(var_r4, u32 *, 0x60) < 0x64U) && ((u32) *var_r5 != 0U)) {
                    var_r0 = 1;
                    *var_r5 = 0U;
                } else {
                    *var_r6 += M2C_FIELD(var_r4, u32 *, 0x60);
                    if ((s32) *var_r6 > 0xE10) {
                        *var_r6 = 0;
                        if ((u32) *var_r5 != 1U) {
                            var_r0 = 1;
                            *var_r5 = 1U;
                        }
                    } else if ((u32) *var_r5 != 0U) {
                        var_r0 = 1;
                        *var_r5 = 0U;
                    }
                }
                break;
            case 2:
                if ((u32) *var_r5 != 0U) {
                    var_r0 = 1;
                    *var_r5 = 0U;
                }
                break;
            case 3:
                if ((u32) *var_r5 != 2U) {
                    var_r0 = 1;
                    *var_r5 = 2U;
                }
                break;
            }
            if ((u32) M2C_FIELD(var_r4, u32 *, 0x64) != 0U) {
                temp_r8_4 = M2C_FIELD(var_r4, u32 *, 0x64) - 1;
                M2C_FIELD(var_r4, u32 *, 0x64) = temp_r8_4;
                if (temp_r8_4 == 0U) {
                    M2C_FIELD(var_r4, s32 *, 0x5C) = 2;
                }
            }
            if ((u8) M2C_FIELD(var_r4, u8 *, 0x68) != 0) {
                if ((u32) M2C_FIELD(var_r4, u32 *, 0x60) < (u8) M2C_FIELD(var_r4, u8 *, 0x68)) {
                    M2C_FIELD(var_r4, s32 *, 0x5C) = 2;
                    M2C_FIELD(var_r4, u8 *, 0x68) = 0U;
                } else {
                    M2C_FIELD(var_r4, u32 *, 0x60) = (u32) (M2C_FIELD(var_r4, u32 *, 0x60) - M2C_FIELD(var_r4, u8 *, 0x68));
                }
            }
        }
        var_r5 += 4;
        var_r6 += 4;
        var_r7 += 1;
        var_ctr -= 1;
    } while (var_ctr != 0);
    if (var_r0 != 0) {
        fn_800AB4FC((u8 *)&lbl_80401C10 + 0x1C0);
    }
}
#pragma pop

/* 0x800F8654 | 0x400 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void fn_800F8654(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg_sp8)
    u8 *arg0;
    s8 arg1;
    s8 arg2;
    u8 *arg3;
    u8 *arg4;
    f32 *arg5;
    f32 *arg6;
    f32 *arg7;
    f32 *arg_sp8;
{
    f32 temp_f2;
    f32 var_f1;
    f32 var_f1_2;
    s32 temp_r0;
    s32 var_r0;
    s32 var_r0_2;
    u8 temp_r30;
    u8 temp_r30_2;
    u8 temp_r3;
    u8 temp_r3_2;
    u8 temp_r3_3;
    u8 temp_r3_4;

    temp_r30 = *arg3;
    if (((s8) temp_r30 < (s32) (arg1 - 2)) || ((s8) temp_r30 > (s32) (arg1 + 2)) || (temp_r30_2 = *arg4, (((s8) temp_r30_2 < (s32) (arg2 - 2)) != 0)) || ((s8) temp_r30_2 > (s32) (arg2 + 2))) {
        temp_r0 = M2C_FIELD(arg0, s32 *, 0x14);
        if (temp_r0 == 0) {
            *arg5 = ((f32) arg1 - *arg7) / (f32) M2C_FIELD(arg0, u8 *, 0x10);
            *arg6 = ((f32) arg2 - *arg_sp8) / (f32) M2C_FIELD(arg0, u8 *, 0x10);
        } else if (temp_r0 == 1) {
            *arg5 = ((f32) arg1 - *arg7) / ((f32) M2C_FIELD(arg0, u8 *, 0x10) * lbl_8047CCD4);
            *arg6 = ((f32) arg2 - *arg_sp8) / ((f32) M2C_FIELD(arg0, u8 *, 0x10) * lbl_8047CCD4);
        }
        M2C_FIELD(arg0, u8 *, 0x11) = 0U;
    }
    *arg3 = (u8) arg1;
    *arg4 = (u8) arg2;
    *arg7 += *arg5;
    *arg_sp8 += *arg6;
    if ((s32) M2C_FIELD(arg0, s32 *, 0x14) == 1) {
        M2C_FIELD(arg0, u8 *, 0x11) = (u8) (M2C_FIELD(arg0, u8 *, 0x11) + 1);
        if ((u8) M2C_FIELD(arg0, u8 *, 0x11) < (u8) M2C_FIELD(arg0, u8 *, 0x10)) {
            *arg5 *= lbl_8047CCD8;
            *arg6 *= lbl_8047CCD8;
            temp_f2 = *arg5;
            if (temp_f2 > lbl_8047CCD0) {
                var_f1 = temp_f2;
            } else {
                var_f1 = -temp_f2;
            }
            if (var_f1 < lbl_8047CCDC) {
                if (temp_f2 > lbl_8047CCD0) {
                    var_r0 = 1;
                } else {
                    var_r0 = -1;
                }
                *arg5 = (f32) var_r0;
            }
            var_f1_2 = *arg6;
            if (var_f1_2 > lbl_8047CCD0) {

            } else {
                var_f1_2 = -var_f1_2;
            }
            if (var_f1_2 < lbl_8047CCDC) {
                if (*arg5 > lbl_8047CCD0) {
                    var_r0_2 = 1;
                } else {
                    var_r0_2 = -1;
                }
                *arg6 = (f32) var_r0_2;
            }
        }
    }
    if (*arg5 < lbl_8047CCD0) {
        temp_r3 = *arg3;
        if (*arg7 < (f32) (s8) temp_r3) {
            *arg7 = (f32) (s8) temp_r3;
        }
    } else {
        temp_r3_2 = *arg3;
        if (*arg7 > (f32) (s8) temp_r3_2) {
            *arg7 = (f32) (s8) temp_r3_2;
        }
    }
    if (*arg6 < lbl_8047CCD0) {
        temp_r3_3 = *arg4;
        if (*arg_sp8 < (f32) (s8) temp_r3_3) {
            *arg_sp8 = (f32) (s8) temp_r3_3;
        }
    } else {
        temp_r3_4 = *arg4;
        if (*arg_sp8 > (f32) (s8) temp_r3_4) {
            *arg_sp8 = (f32) (s8) temp_r3_4;
        }
    }
}
#pragma pop

/* 0x800F8A54 | 0x708 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void fn_800F8A54(arg0)
    u8 *arg0;
{
    s32 mode;
    s32 x;
    s32 y;
    s32 len;
    s32 sq;
    s32 radius;
    s32 limitSq;

    mode = *(s32 *)(arg0 + 0x08);
    if (mode == 0) {
        *(s8 *)(arg0 + 0x58) = (s8)*(f32 *)(arg0 + 0x48);
        *(s8 *)(arg0 + 0x59) = (s8)*(f32 *)(arg0 + 0x4C);
        *(s8 *)(arg0 + 0x5A) = (s8)*(f32 *)(arg0 + 0x50);
        *(s8 *)(arg0 + 0x5B) = (s8)*(f32 *)(arg0 + 0x54);
        return;
    }

    if (mode != 2) {
        return;
    }

    x = (s8)arg0[0x26];
    y = (s8)arg0[0x27];
    if (x > -10 && x < 10) {
        x = 0;
    } else if (x > 0) {
        x -= 10;
    } else {
        x += 10;
    }
    if (y > -10 && y < 10) {
        y = 0;
    } else if (y > 0) {
        y -= 10;
    } else {
        y += 10;
    }
    radius = 0x38;
    limitSq = 0xC40;
    sq = x * x + y * y;
    if (sq > limitSq) {
        len = 1;
        while ((len + 1) * (len + 1) <= sq) {
            len++;
        }
        x = (x * radius) / len;
        y = (y * radius) / len;
    }
    arg0[0x26] = (u8)x;
    arg0[0x27] = (u8)y;

    x = (s8)arg0[0x28];
    y = (s8)arg0[0x29];
    if (x > -10 && x < 10) {
        x = 0;
    } else if (x > 0) {
        x -= 10;
    } else {
        x += 10;
    }
    if (y > -10 && y < 10) {
        y = 0;
    } else if (y > 0) {
        y -= 10;
    } else {
        y += 10;
    }
    radius = 0x2C;
    limitSq = 0x790;
    sq = x * x + y * y;
    if (sq > limitSq) {
        len = 1;
        while ((len + 1) * (len + 1) <= sq) {
            len++;
        }
        x = (x * radius) / len;
        y = (y * radius) / len;
    }
    arg0[0x28] = (u8)x;
    arg0[0x29] = (u8)y;

    x = (s8)*(f32 *)(arg0 + 0x48);
    y = (s8)*(f32 *)(arg0 + 0x4C);
    if (x > -10 && x < 10) {
        x = 0;
    } else if (x > 0) {
        x -= 10;
    } else {
        x += 10;
    }
    if (y > -10 && y < 10) {
        y = 0;
    } else if (y > 0) {
        y -= 10;
    } else {
        y += 10;
    }
    radius = 0x38;
    limitSq = 0xC40;
    sq = x * x + y * y;
    if (sq > limitSq) {
        len = 1;
        while ((len + 1) * (len + 1) <= sq) {
            len++;
        }
        x = (x * radius) / len;
        y = (y * radius) / len;
    }
    *(s8 *)(arg0 + 0x58) = (s8)x;
    *(s8 *)(arg0 + 0x59) = (s8)y;

    x = (s8)*(f32 *)(arg0 + 0x50);
    y = (s8)*(f32 *)(arg0 + 0x54);
    if (x > -10 && x < 10) {
        x = 0;
    } else if (x > 0) {
        x -= 10;
    } else {
        x += 10;
    }
    if (y > -10 && y < 10) {
        y = 0;
    } else if (y > 0) {
        y -= 10;
    } else {
        y += 10;
    }
    radius = 0x2C;
    limitSq = 0x790;
    sq = x * x + y * y;
    if (sq > limitSq) {
        len = 1;
        while ((len + 1) * (len + 1) <= sq) {
            len++;
        }
        x = (x * radius) / len;
        y = (y * radius) / len;
    }
    *(s8 *)(arg0 + 0x5A) = (s8)x;
    *(s8 *)(arg0 + 0x5B) = (s8)y;
}
#pragma pop

/* 0x800F915C | 0xB4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800F915C(u32 key) {
    u8* entry;
    u32 i;
    u8 skip;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) == 0 || *(u32*)(entry + 0x8) != key) continue;
        skip = 0;
        if (*(u32*)(entry + 0x10) != 0) {
            if ((u8)((u32 (*)(u32))(*(u32*)(entry + 0x10)))(*(u32*)(entry + 0xC)) == 0) {
                skip = 1;
            }
        }
        if (!skip) {
            if (*(u16*)(entry + 0x0) != 0) {
                GSmemLock(*(u16*)(entry + 0x0));
                GSmemFree(*(u16*)(entry + 0x0));
                *(u16*)(entry + 0x0) = 0;
            }
            *(u32*)(entry + 0x4) = 0;
        }
    }
}
#pragma pop

/* 0x800F9210 | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800F9210(u32 key1, u32 key2) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0x8) == key1 && *(u32*)(entry + 0xC) == key2) {
            if (*(u32*)(entry + 0x10) != 0) {
                if ((u8)((u32 (*)(u32, u32, u32))(*(u32*)(entry + 0x10)))(*(u32*)(entry + 0x4), *(u32*)(entry + 0x8), *(u32*)(entry + 0xC)) == 0) {
                    return;
                }
            }
            if (*(u16*)(entry + 0x0) != 0) {
                GSmemLock(*(u16*)(entry + 0x0));
                GSmemFree(*(u16*)(entry + 0x0));
                *(u16*)(entry + 0x0) = 0;
            }
            *(u32*)(entry + 0x4) = 0;
            return;
        }
    }
}
#pragma pop

/* 0x800F92D4 | 0x44 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
u32 fn_800F92D4(u32 key) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0xC) == key)
            return *(u32*)(entry + 0x4);
    }
    return 0;
}
#pragma pop

/* 0x800F9318 | 0x60 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma push
#pragma peephole off
#pragma optimization_level 2
u32 GSresGetResource(u32 key1, u32 key2) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--, entry += 0x14) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0x8) == key1 && *(u32*)(entry + 0xC) == key2)
            return *(u32*)(entry + 0x4);
    }
    return 0;
}
#pragma pop
#pragma pop

/* 0x800F9378 | 0xA0 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSresRegisterResource(u32 fn, u32 key1, u32 key2, u32 val) {
    u8* arr;
    u8* p;
    u32 i;

    arr = (u8*)lbl_8047AC5C;
    i = lbl_8047AC60;
    p = arr;
    for (; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) != 0 && *(u32*)(p + 0x8) == key1 && *(u32*)(p + 0xC) == key2)
            return;
    }
    p = arr;
    i = lbl_8047AC60;
    for (; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) == 0) break;
    }
    if (i == 0) return;
    *(u16*)(p + 0x0) = 0;
    *(u32*)(p + 0x4) = fn;
    *(u32*)(p + 0x8) = key1;
    *(u32*)(p + 0xC) = key2;
    *(u32*)(p + 0x10) = val;
}
#pragma pop

/* 0x800F9418 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void* GSresAllocResourceAlign(u32 align, u32 size, u32 key1, u32 key2, u32 val) {
    u8* arr;
    u8* p;
    u8* slot;
    u32 i;
    u32 count;

    arr = (u8*)lbl_8047AC5C;
    count = lbl_8047AC60;
    p = arr;
    for (i = count; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) != 0 && *(u32*)(p + 0x8) == key1 && *(u32*)(p + 0xC) == key2)
            return NULL;
    }
    slot = arr;
    for (i = count; i > 0; i--, slot += 0x14) {
        if (*(u32*)(slot + 0x4) == 0) break;
    }
    if (i == 0) return NULL;
    *(u16*)slot = (u16)GSmemAlloc(align, size);
    if (*(u16*)slot == 0) return NULL;
    *(u32*)(slot + 0x4) = (u32)GSmemGetPtr(*(u16*)slot);
    if (*(u32*)(slot + 0x4) == 0) {
        GSmemFree(*(u16*)slot);
        return NULL;
    }
    *(u32*)(slot + 0x8) = key1;
    *(u32*)(slot + 0xC) = key2;
    *(u32*)(slot + 0x10) = val;
    return (void*)*(u32*)(slot + 0x4);
}
#pragma pop

/* 0x800F9544 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void* GSresAllocResource(u32 size, u32 key1, u32 key2, u32 val) {
    u8* arr;
    u8* p;
    u8* slot;
    u32 i;
    u32 count;

    arr = (u8*)lbl_8047AC5C;
    count = lbl_8047AC60;
    p = arr;
    for (i = count; i > 0; i--, p += 0x14) {
        if (*(u32*)(p + 0x4) != 0 && *(u32*)(p + 0x8) == key1 && *(u32*)(p + 0xC) == key2)
            return NULL;
    }
    slot = arr;
    for (i = count; i > 0; i--, slot += 0x14) {
        if (*(u32*)(slot + 0x4) == 0) break;
    }
    if (i == 0) return NULL;
    *(u16*)slot = (u16)GSmemAllocRaw(size);
    if (*(u16*)slot == 0) return NULL;
    *(u32*)(slot + 0x4) = (u32)GSmemGetPtr(*(u16*)slot);
    if (*(u32*)(slot + 0x4) == 0) {
        GSmemFree(*(u16*)slot);
        return NULL;
    }
    *(u32*)(slot + 0x8) = key1;
    *(u32*)(slot + 0xC) = key2;
    *(u32*)(slot + 0x10) = val;
    return (void*)*(u32*)(slot + 0x4);
}
#pragma pop

/* 0x800F9670 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSresInit(u32 count) {
    u16 handle;
    u32 off;
    u32 i;

    lbl_8047AC60 = count;
    handle = GSmemAllocRaw(count * 0x14);
    lbl_8047AC58 = handle;
    if ((handle & 0xFFFF) == 0) return;
    lbl_8047AC5C = (u32)GSmemGetPtr(handle);
    off = 0;
    i = 0;
    while (i < lbl_8047AC60) {
        u32 storeoff = off + 0x4;
        off += 0x14;
        i++;
        *(u32*)(lbl_8047AC5C + storeoff) = 0;
    }
}
#pragma pop

/* 0x800F96E4 | 0x408 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
u8 *fn_800F96E4(arg0, arg1, arg2)
    u8 *arg0;
    s32 arg1;
    u32 arg2;
{
    u8 *mgr;
    u8 *bank;
    u8 *text;
    u8 *entry;
    u8 *node;
    u8 *work;
    u8 *dst;
    u8 *dstStart;
    u8 *copy;
    u8 *copyEnd;
    u8 *table;
    u16 *ip;
    u16 *controlIp;
    u16 code;
    u16 control;
    u16 lo;
    u16 hi;
    u32 mid;
    u32 subKey;
    u32 count;
    u32 countStart;
    u32 maxBytes;
    u32 flags;
    u32 result;
    u32 mode;
    u8 savedDepth;
    u16 fontId;
    s32 i;

    if (arg2 == 0 || arg0 == NULL || arg1 <= 0) {
        return NULL;
    }

    mgr = (u8 *)lbl_80478B08;
    bank = (u8 *)*(u32 *)(mgr + 0x08);
    text = NULL;
    while (bank != NULL) {
        if (*(u16 *)bank == (u16)(arg2 >> 0x14)) {
            lo = 0;
            hi = *(u16 *)(bank + 0x04);
            subKey = arg2 & 0xFFFFF;
            while (lo < hi) {
                mid = ((u32)lo + (u32)hi) >> 1;
                entry = bank + 0x10 + mid * 8;
                if (*(u32 *)entry == subKey) {
                    text = bank + *(s32 *)(entry + 4);
                    break;
                }
                if (*(u32 *)entry < subKey) {
                    lo = (u16)(mid + 1);
                } else {
                    hi = (u16)mid;
                }
            }
            if (text != NULL) {
                break;
            }
        }
        bank = (u8 *)*(u32 *)(bank + 0x08);
    }
    if (text == NULL) {
        return NULL;
    }

    work = (u8 *)&lbl_80401DE0;
    memset(work, 0, 0x68);
    work[0] = 1;
    work[1] = 1;
    *(f32 *)(work + 0x60) = lbl_8047CD08;
    *(f32 *)(work + 0x64) = lbl_8047CD08;
    *(s32 *)(work + 0x24) = -1;
    *(u32 *)(work + 0x28) = (u32)text;
    *(u32 *)(work + 0x2C) = (u32)text;
    *(u32 *)(work + 0x30) = (u32)text;
    fontId = bank[3];
    *(u16 *)(work + 0x20) = fontId;
    *(u32 *)(work + 0x1C) = arg2;

    for (i = 0; i < *(u16 *)(mgr + 0x04); i++) {
        entry = (u8 *)*(u32 *)(mgr + 0x24) + i * 8;
        if (*(u16 *)entry == fontId) {
            work[0x22] = entry[2];
            work[0x23] = entry[3];
            if (fontId == 0) {
                *(s8 *)(work + 0x42) = 0xB;
            } else if (fontId == 1) {
                *(s8 *)(work + 0x42) = 6;
            } else {
                *(s8 *)(work + 0x42) = (s8)((lbl_8047CD20 * (f64)entry[3]) + lbl_8047CD18);
            }
            break;
        }
    }

    dst = arg0;
    count = 0;
    maxBytes = (arg1 - 1) * 2;
    for (;;) {
        dstStart = dst;
        countStart = count;
        ip = *(u16 **)(work + 0x30);
        code = *ip;
        if (code == 0) {
            break;
        }
        *(u16 **)(work + 0x30) = ip + 2;

        count += 2;
        if (count > maxBytes) {
            dst = dstStart;
            break;
        }
        *(u16 *)dst = code;
        dst += 2;

        if (code != 0xFFFF) {
            continue;
        }

        controlIp = *(u16 **)(work + 0x30);
        control = (u8)*controlIp;
        *(u16 **)(work + 0x30) = controlIp + 1;
        count++;
        if (count > maxBytes) {
            dst = dstStart;
            break;
        }
        *dst++ = (u8)control;

        copy = (u8 *)(controlIp + 1);
        savedDepth = work[0x40];
        table = (u8 *)*(u32 *)(mgr + 0x28);
        if (table != NULL) {
            entry = table + control * 8;
            if (work[1] == 0) {
                flags = (entry[0] >> 4) & 1;
            } else {
                flags = (entry[0] >> 3) & 1;
            }
            if (flags != 0 && *(u32 *)(entry + 4) != 0) {
                result = ((u32 (*)(u8 *))*(u32 *)(entry + 4))(work);
                mode = (entry[0] >> 6) & 3;
                if (mode != 0 && result != 0) {
                    if (mode == 1) {
                        *(u32 *)(work + 0x30) = result;
                    } else if (mode == 2) {
                        node = (u8 *)*(u32 *)(mgr + 0x08);
                        subKey = result & 0xFFFFF;
                        while (node != NULL) {
                            if (*(u16 *)node == (u16)(result >> 0x14)) {
                                lo = 0;
                                hi = *(u16 *)(node + 0x04);
                                while (lo < hi) {
                                    mid = ((u32)lo + (u32)hi) >> 1;
                                    entry = node + 0x10 + mid * 8;
                                    if (*(u32 *)entry == subKey) {
                                        *(u32 *)(work + 0x30) = (u32)(node + *(s32 *)(entry + 4));
                                        node = NULL;
                                        break;
                                    }
                                    if (*(u32 *)entry < subKey) {
                                        lo = (u16)(mid + 1);
                                    } else {
                                        hi = (u16)mid;
                                    }
                                }
                                if (node == NULL) {
                                    break;
                                }
                            }
                            node = (u8 *)*(u32 *)(node + 0x08);
                        }
                    }

                    if ((s8)work[0x40] >= 3) {
                        GSlogWrite((const char *)lbl_80271700, lbl_80315678);
                    } else {
                        *(u32 *)(work + 0x34 + (s8)work[0x40] * 4) = (u32)copy;
                        work[0x40]++;
                    }
                }
            }
        }

        if (savedDepth == work[0x40]) {
            copyEnd = *(u8 **)(work + 0x30);
            count += (u32)(copyEnd - copy);
            if (count > maxBytes) {
                dst = dstStart;
                count = countStart;
                continue;
            }
            while (copy < copyEnd) {
                *dst++ = *copy++;
            }
        } else {
            dst = dstStart;
            count = countStart;
        }
    }

    *dst = 0;
    return arg0;
}
#pragma pop

/* 0x800F9AEC | 0x118 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
u32 fn_800F9AEC(void* outbuf, u16* src, s32 mode) {
    u8* out;
    register u16* table;
    register s32 idx;
    register u32 count;
    register u16* p;
    register u32 ch;
    u32 outch;

    out = (u8*)outbuf;

    switch (mode) {
    case 1:
        break;
    case 7:
    case 9:
    default:
        goto use_second_table;
    }

    count = 0;
    if (src != NULL) goto first_have_src;
    goto done_first;
first_have_src:
        table = (u16*)lbl_80271300;
        goto first_cond;
first_loop:
        p = table;
        idx = 0;
        goto first_scan_check;
first_scan_next:
        idx++;
        p++;
        if (idx >= 0x100) {
            idx = 0xb7;
            goto first_found;
        }
first_scan_check:
        if ((u32)ch != *p) goto first_scan_next;
first_found:
        if (out != NULL) {
            outch = (u8)idx;
            *out = outch;
            out++;
        }
        count++;
        src++;
first_cond:
        ch = *src;
        if (ch != 0) goto first_loop;
done_first:
    return count;

use_second_table:
    count = 0;
    if (src != NULL) goto second_have_src;
    goto done_second;
second_have_src:
    table = (u16*)lbl_80271500;
    goto second_cond;
second_loop:
    p = table;
    idx = 0;
    goto second_scan_check;
second_scan_next:
    idx++;
    p++;
    if (idx >= 0x100) {
        idx = 0xb7;
        goto second_found;
    }
second_scan_check:
    if ((u32)ch != *p) goto second_scan_next;
second_found:
    if (out != NULL) {
        outch = (u8)idx;
        *out = outch;
        out++;
    }
    count++;
    src++;
second_cond:
    ch = *src;
    if (ch != 0) goto second_loop;
done_second:
    return count;
}
#pragma pop

/* 0x800F9C04 | 0x100 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
u32 fn_800F9C04(void* outbuf, u8* src, u32 count, u32 mode) {
    u16* out;
    u16* table;
    u32 total;
    u8 b;

    out = (u16*)outbuf;
    total = 0;

    if (mode == 1) {
        if (src == NULL) {
            if (out != NULL) *out = 0;
            return total;
        }
        table = (u16*)lbl_80271300;
        while (count > 0 && (b = *src) != 0xFF) {
            if (out != NULL) {
                *out = table[b];
                out++;
            }
            total++;
            src++;
            count--;
        }
        if (out != NULL) *out = 0;
    } else {
        if (src == NULL) {
            if (out != NULL) *out = 0;
            return total;
        }
        table = (u16*)lbl_80271500;
        while (count > 0 && (b = *src) != 0xFF) {
            if (out != NULL) {
                *out = table[b];
                out++;
            }
            total++;
            src++;
            count--;
        }
        if (out != NULL) *out = 0;
    }
    return total;
}
#pragma pop

/* 0x800F9D04 | 0x20 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GScharMakeFromSJIS(void) {
    fn_80080ED8();
}
#pragma pop

/* 0x800F9D24 | 0x14C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma push
#pragma peephole off
#pragma optimization_level 2
void* fn_800F9D24(u16* dst, u16* src, s32 maxlen) {
    s32 r;
    s32 i;

    if (maxlen <= 0) return dst;
    r = (_msgGetSize__FPCUs(src) + 1) >> 1;
    if (r >= maxlen) r = maxlen - 1;
    memcpy(dst, src, r * 2);
    i = r;
    r = maxlen - r;
    if (r <= 0) return dst;
    while (r > 0) {
        dst[i] = 0;
        i++;
        r--;
    }
    return dst;
}
#pragma pop
#pragma pop

/* 0x800F9E70 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma peephole off
u8* GScharCpy(u8* dst, u8* src) {
    extern u32 _msgGetSize__FPCUs(u8* a);
    if (dst == NULL) { return NULL; }
    if (src == NULL) { *(u16*)dst = 0; }
    else { memcpy(dst, src, _msgGetSize__FPCUs(src)); }
    return dst;
}
#pragma peephole on
#pragma pop

/* 0x800F9EE4 | 0x180 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
s32 fn_800F9EE4(void* str1, void* str2) {
    s32 len1;
    s32 len2;
    u16* p1;
    u16* p2;
    u32 i;
    u16 c1;
    u16 c2;
    s32 n;

    len1 = _msgGetLength__FPCUs(str1);
    len2 = _msgGetLength__FPCUs(str2);

    if (len1 == len2) {
        p1 = (u16*)str1;
        p2 = (u16*)str2;
        i = 0;
        n = len1;
        while (n > 0) {
            c1 = *p1;
            c2 = *p2;
            if (c1 != c2) {
                c1 = ((u16*)str1)[i];
                c2 = ((u16*)str2)[i];
                if (c1 > c2) return 1;
                return -1;
            }
            p1++;
            p2++;
            i++;
            n--;
        }
        return 0;
    } else if (len1 > len2) {
        p1 = (u16*)str2;
        p2 = (u16*)str1;
        i = 0;
        n = len2;
        while (n > 0) {
            c1 = *p2;
            c2 = *p1;
            if (c1 != c2) {
                c1 = ((u16*)str1)[i];
                c2 = ((u16*)str2)[i];
                if (c1 > c2) return 1;
                return -1;
            }
            p1++;
            p2++;
            i++;
            n--;
        }
        return 1;
    } else {
        p1 = (u16*)str2;
        p2 = (u16*)str1;
        i = 0;
        n = len1;
        while (n > 0) {
            c1 = *p2;
            c2 = *p1;
            if (c1 != c2) {
                c1 = ((u16*)str1)[i];
                c2 = ((u16*)str2)[i];
                if (c1 > c2) return 1;
                return -1;
            }
            p1++;
            p2++;
            i++;
            n--;
        }
        return -1;
    }
}
#pragma pop

/* 0x800FA064 | 0xFC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800FA064(void* obj) {
    u8* o;
    u32 rv;
    s16 r5;
    u8 type;
    f32 base;
    f64 bias;

    o = (u8*)obj;
    if (*(s16*)(o + 0x18) == 0) return;
    rv = GSmsgGetRect(*(void**)(o + 0x1C));
    r5 = (s16)(rv >> 16);
    type = *(u8*)(o + 0x4A);
    base = *(f32*)(o + 0x4);
    bias = lbl_8047CD10;

    if (type == 0) {
        *(f32*)(o + 0xC) = base;
    } else if (type == 1) {
        s16 target;
        s32 diff;
        target = *(s16*)(o + 0x18);
        diff = (s32)(target - r5);
        diff = (diff + (diff >> 31)) >> 1;
        *(f32*)(o + 0xC) = base + (f32)((f64)diff - bias + bias);
    } else if (type < 4) {
        s16 target;
        target = *(s16*)(o + 0x18);
        *(f32*)(o + 0xC) = base + (f32)target - (f32)r5;
    }
}
#pragma pop

/* 0x800FA160 | 0x5C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
#pragma peephole off
void GSmsgSetColor(void* obj) {
    u8 clr[8];
    u32 init = lbl_8047CD04;
    u32 color = *(u32*)((u8*)obj + 0x24);
    *(u32*)(&clr[4]) = init;
    clr[4] = (u8)(color >> 24);
    clr[5] = (u8)((color >> 16) & 0xFF);
    clr[6] = (u8)((color >> 8) & 0xFF);
    clr[7] = (u8)(color & 0xFF);
    *(u32*)(&clr[0]) = *(u32*)(&clr[4]);
    fn_800DBEB4(0, &clr[0]);
}
#pragma peephole on
#pragma pop

/* 0x800FA1BC | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSmsgSetFontInfo(void* obj) {
    u8* o;
    u8* head;
    u32 count;
    u32 offset;
    u8* arr;
    u8* entry;
    u16 val;

    o = (u8*)obj;
    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    if ((s32)count <= 0) return;
    offset = 0;
    do {
        arr = (u8*)*(u32*)(head + 0x24);
        entry = arr + offset;
        if (*(u16*)entry == *(u16*)(o + 0x20)) {
            *(u8*)(o + 0x22) = entry[2];
            *(u8*)(o + 0x23) = entry[3];
            val = *(u16*)(o + 0x20);
            if (val == 0) {
                *(u8*)(o + 0x42) = 0xB;
                return;
            } else if (val == 1) {
                *(u8*)(o + 0x42) = 6;
                return;
            } else {
                *(s8*)(o + 0x42) = (s8)(s32)(lbl_8047CD20 * ((f64)(u32)entry[3] - lbl_8047CD28) + lbl_8047CD18);
                return;
            }
        }
        offset += 8;
        count--;
    } while (count > 0);
}
#pragma pop

/* 0x800FA280 | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void* GSmsgGetGSchar(u32 key) {
    u8* head;
    u16 group;
    u32 sub;
    u8* node;
    u32 lo;
    u32 hi;
    u32 mid;
    u32 count;
    u8* arr;
    u32 val;

    if (key == 0) return NULL;

    head = (u8*)lbl_80478B08;
    group = (u16)(key >> 20);
    sub = key & 0xFFFFF;

    node = (u8*)*(u32*)(head + 0x8);
    while (node != NULL) {
        if (*(u16*)(node + 0x0) == group) {
            count = *(u16*)(node + 0x4);
            arr = node + 0x10;
            lo = 0;
            hi = count;
            while (lo < hi) {
                mid = (lo + hi) >> 1;
                val = *(u32*)(arr + mid * 8);
                if (val == sub) {
                    u32 offset = *(u32*)(arr + mid * 8 + 4);
                    return node + offset;
                }
                if (val < sub) lo = mid + 1;
                else hi = mid;
            }
        }
        node = (u8*)*(u32*)(node + 0x8);
    }
    return NULL;
}
#pragma pop

/* 0x800FA314 | 0xBC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
s32 GSmsgGetLength(u32 key) {
    u8* head;
    u16 group;
    u32 sub;
    u8* node;
    u32 lo;
    u32 hi;
    u32 mid;
    u32 count;
    u8* arr;
    u32 val;
    void* result;

    if (key == 0) return _msgGetLength__FPCUs(NULL);

    head = (u8*)lbl_80478B08;
    group = (u16)(key >> 20);
    sub = key & 0xFFFFF;

    node = (u8*)*(u32*)(head + 0x8);
    result = NULL;
    while (node != NULL) {
        if (*(u16*)(node + 0x0) == group) {
            count = *(u16*)(node + 0x4);
            arr = node + 0x10;
            lo = 0;
            hi = count;
            while (lo < hi) {
                mid = (lo + hi) >> 1;
                val = *(u32*)(arr + mid * 8);
                if (val == sub) {
                    u32 offset = *(u32*)(arr + mid * 8 + 4);
                    result = node + offset;
                    return _msgGetLength__FPCUs(result);
                }
                if (val < sub) lo = mid + 1;
                else hi = mid;
            }
        }
        node = (u8*)*(u32*)(node + 0x8);
    }
    return _msgGetLength__FPCUs(NULL);
}
#pragma pop

/* 0x800FA3D0 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
s32 GSmsgIsCheck(u32 key) {
    u8* head;
    u32 count;
    u8* entry;
    u8 i;

    head = (u8*)lbl_80478B08;
    count = *(u16*)head;

    for (i = 0; (u32)(i & 0xFF) < count; i++) {
        entry = (u8*)*(u32*)(head + 0x20) + (u32)(i & 0xFF) * 0x68;
        if (*(u8*)(entry + 0x0) != 0 && *(u32*)(entry + 0x1C) == key) {
            if (*(u8*)(entry + 0x0) == 1) return 1;
            return 0;
        }
    }
    return 0;
}
#pragma pop

/* 0x800FA444 | 0x654 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 GSmsgGetRect(arg0)
    u32 arg0;
{
    u8 *mgr;
    u8 *bank;
    u8 *text;
    u8 *entry;
    u8 *work;
    u8 *table;
    u8 *node;
    u16 *ip;
    u16 code;
    u16 control;
    u16 fontId;
    u16 lo;
    u16 hi;
    u32 mid;
    u32 subKey;
    u32 flags;
    u32 result;
    u32 mode;
    s16 maxX;
    s16 maxY;
    u8 lineStart;
    void *fontInfo;
    s32 i;

    if (arg0 == 0) {
        return 0;
    }

    mgr = (u8 *)lbl_80478B08;
    bank = (u8 *)*(u32 *)(mgr + 0x08);
    text = NULL;
    while (bank != NULL) {
        if (*(u16 *)bank == (u16)(arg0 >> 0x14)) {
            lo = 0;
            hi = *(u16 *)(bank + 0x04);
            subKey = arg0 & 0xFFFFF;
            while (lo < hi) {
                mid = ((u32)lo + (u32)hi) >> 1;
                entry = bank + 0x10 + mid * 8;
                if (*(u32 *)entry == subKey) {
                    text = bank + *(s32 *)(entry + 4);
                    break;
                }
                if (*(u32 *)entry < subKey) {
                    lo = (u16)(mid + 1);
                } else {
                    hi = (u16)mid;
                }
            }
            if (text != NULL) {
                break;
            }
        }
        bank = (u8 *)*(u32 *)(bank + 0x08);
    }
    if (text == NULL) {
        return 0;
    }

    work = (u8 *)&lbl_80401E48;
    memset(work, 0, 0x68);
    work[0] = 1;
    work[1] = 1;
    *(f32 *)(work + 0x60) = lbl_8047CD08;
    *(f32 *)(work + 0x64) = lbl_8047CD08;
    *(s32 *)(work + 0x24) = -1;
    *(u32 *)(work + 0x28) = (u32)text;
    *(u32 *)(work + 0x2C) = (u32)text;
    *(u32 *)(work + 0x30) = (u32)text;
    fontId = bank[3];
    *(u16 *)(work + 0x20) = fontId;
    *(u32 *)(work + 0x1C) = arg0;

    for (i = 0; i < *(u16 *)(mgr + 0x04); i++) {
        entry = (u8 *)*(u32 *)(mgr + 0x24) + i * 8;
        if (*(u16 *)entry == fontId) {
            work[0x22] = entry[2];
            work[0x23] = entry[3];
            if (fontId == 0) {
                *(s8 *)(work + 0x42) = 0xB;
            } else if (fontId == 1) {
                *(s8 *)(work + 0x42) = 6;
            } else {
                *(s8 *)(work + 0x42) = (s8)((lbl_8047CD20 * (f64)entry[3]) + lbl_8047CD18);
            }
            break;
        }
    }

    maxX = 0;
    maxY = 0;
    lineStart = 0;
    for (;;) {
        ip = *(u16 **)(work + 0x30);
        code = *ip;
        if (code == 0) {
            if ((s8)work[0x40] == 0) {
                break;
            }
            work[0x40]--;
            *(u32 *)(work + 0x30) = *(u32 *)(work + 0x34 + (s8)work[0x40] * 4);
            continue;
        }

        *(u16 **)(work + 0x30) = ip + 2;
        if (code == 0xFFFF) {
            ip = *(u16 **)(work + 0x30);
            control = (u8)*ip;
            *(u16 **)(work + 0x30) = ip + 1;

            if (control == 3) {
                *(f32 *)(work + 0x0C) += (f32)work[0x22];
                if ((f32)maxX < *(f32 *)(work + 0x0C)) {
                    maxX = (s16)*(f32 *)(work + 0x0C);
                }
            }

            table = (u8 *)*(u32 *)(mgr + 0x28);
            if (table != NULL) {
                entry = table + control * 8;
                if (work[1] == 0) {
                    flags = (entry[0] >> 4) & 1;
                } else {
                    flags = (entry[0] >> 3) & 1;
                }
                if (flags != 0 && *(u32 *)(entry + 4) != 0) {
                    result = ((u32 (*)(u8 *))*(u32 *)(entry + 4))(work);
                    mode = (entry[0] >> 6) & 3;
                    if (mode != 0 && result != 0) {
                        if (mode == 1) {
                            *(u32 *)(work + 0x30) = result;
                        } else if (mode == 2) {
                            node = (u8 *)*(u32 *)(mgr + 0x08);
                            subKey = result & 0xFFFFF;
                            while (node != NULL) {
                                if (*(u16 *)node == (u16)(result >> 0x14)) {
                                    lo = 0;
                                    hi = *(u16 *)(node + 0x04);
                                    while (lo < hi) {
                                        mid = ((u32)lo + (u32)hi) >> 1;
                                        entry = node + 0x10 + mid * 8;
                                        if (*(u32 *)entry == subKey) {
                                            *(u32 *)(work + 0x30) = (u32)(node + *(s32 *)(entry + 4));
                                            node = NULL;
                                            break;
                                        }
                                        if (*(u32 *)entry < subKey) {
                                            lo = (u16)(mid + 1);
                                        } else {
                                            hi = (u16)mid;
                                        }
                                    }
                                    if (node == NULL) {
                                        break;
                                    }
                                }
                                node = (u8 *)*(u32 *)(node + 0x08);
                            }
                        }
                        if ((s8)work[0x40] >= 3) {
                            GSlogWrite((const char *)lbl_80271700, lbl_80315678);
                        } else {
                            *(u32 *)(work + 0x34 + (s8)work[0x40] * 4) = (u32)ip + 2;
                            work[0x40]++;
                        }
                    }
                }
            }

            if (lineStart != 0 && *(f32 *)(work + 0x0C) == *(f32 *)(work + 0x04)) {
                *(f32 *)(work + 0x0C) += (f32)work[0x22];
            }
        } else if (work[0x4B] != 2) {
            if (code == 0x20) {
                *(f32 *)(work + 0x14) = (f32)((work[0x22] >> 1) * *(f32 *)(work + 0x60));
            } else {
                fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(work, code, NULL, NULL);
                if (fontInfo == NULL) {
                    *(f32 *)(work + 0x14) = (f32)work[0x22] * *(f32 *)(work + 0x60);
                } else {
                    *(f32 *)(work + 0x14) = (f32)((u8 *)fontInfo)[2] * *(f32 *)(work + 0x60);
                }
            }

            if ((s8)work[0x41] == 0) {
                if (code == 0x300C) {
                    lineStart = 1;
                }
                if (code == 0x300D) {
                    lineStart = 0;
                }
            }

            *(f32 *)(work + 0x0C) += *(f32 *)(work + 0x14);
            if ((f32)maxX < *(f32 *)(work + 0x0C)) {
                maxX = (s16)*(f32 *)(work + 0x0C);
            }
            if ((f32)maxY < *(f32 *)(work + 0x10)) {
                maxY = (s16)*(f32 *)(work + 0x10);
            }
        }
    }

    return ((maxX - 1) << 0x10) | (s16)((f32)maxY + ((f32)work[0x23] * *(f32 *)(work + 0x64)) + lbl_8047CD08);
}
#pragma pop

/* 0x800FAA98 | 0x460 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void GSmsgInitRuby(arg0)
    u8 *arg0;
{
    u16 *savedIp;
    u16 *ip;
    u16 *resume0;
    u16 *resume1;
    u16 *resume2;
    u16 code;
    u16 control;
    u8 savedDepth;
    u8 savedMode;
    u8 savedFlag;
    u8 pass;
    u8 glyphWidth;
    u8 count0;
    u8 count1;
    s16 width0;
    s16 width1;
    u8 *mgr;
    u8 *table;
    u8 *entry;
    u8 *node;
    u32 flags;
    u32 result;
    u32 mode;
    u32 subKey;
    u16 lo;
    u16 hi;
    u32 mid;
    void *fontInfo;

    savedIp = *(u16 **)(arg0 + 0x30);
    savedDepth = arg0[0x40];
    savedMode = arg0[0x45];
    savedFlag = arg0[1];
    resume0 = *(u16 **)(arg0 + 0x34);
    resume1 = *(u16 **)(arg0 + 0x38);
    resume2 = *(u16 **)(arg0 + 0x3C);

    *(u32 *)(arg0 + 0x54) = 0;
    arg0[0x58] = 0;
    arg0[0x59] = 0;
    arg0[1] = 0;
    arg0[0x45] = 1;
    arg0[0x4B] = 1;

    mgr = (u8 *)lbl_80478B08;
    count0 = 0;
    count1 = 0;
    width0 = 0;
    width1 = 0;

    for (pass = 0; pass < 2; pass++) {
        for (;;) {
            ip = *(u16 **)(arg0 + 0x30);
            code = *ip;
            if (code == 0) {
                if ((s8)arg0[0x40] == 0) {
                    break;
                }
                arg0[0x40]--;
                *(u32 *)(arg0 + 0x30) = *(u32 *)(arg0 + 0x34 + (s8)arg0[0x40] * 4);
                continue;
            }

            *(u16 **)(arg0 + 0x30) = ip + 2;
            if (code == 0xFFFF) {
                ip = *(u16 **)(arg0 + 0x30);
                control = (u8)*ip;
                *(u16 **)(arg0 + 0x30) = ip + 1;

                table = (u8 *)*(u32 *)(mgr + 0x28);
                if (table != NULL) {
                    entry = table + control * 8;
                    if (arg0[1] == 0) {
                        flags = (entry[0] >> 4) & 1;
                    } else {
                        flags = (entry[0] >> 3) & 1;
                    }

                    if (flags != 0 && *(u32 *)(entry + 4) != 0) {
                        result = ((u32 (*)(u8 *))*(u32 *)(entry + 4))(arg0);
                        mode = (entry[0] >> 6) & 3;
                        if (mode != 0 && result != 0) {
                            if (mode == 1) {
                                *(u32 *)(arg0 + 0x30) = result;
                            } else if (mode == 2) {
                                node = NULL;
                                subKey = result & 0xFFFFF;
                                node = (u8 *)*(u32 *)(mgr + 0x08);
                                while (node != NULL) {
                                    if (*(u16 *)node == (u16)(result >> 0x14)) {
                                        lo = 0;
                                        hi = *(u16 *)(node + 0x04);
                                        while (lo < hi) {
                                            mid = ((u32)lo + (u32)hi) >> 1;
                                            entry = node + 0x10 + mid * 8;
                                            if (*(u32 *)entry == subKey) {
                                                *(u32 *)(arg0 + 0x30) = (u32)(node + *(s32 *)(entry + 4));
                                                node = NULL;
                                                break;
                                            }
                                            if (*(u32 *)entry < subKey) {
                                                lo = (u16)(mid + 1);
                                            } else {
                                                hi = (u16)mid;
                                            }
                                        }
                                        if (node == NULL) {
                                            break;
                                        }
                                    }
                                    node = (u8 *)*(u32 *)(node + 0x08);
                                }
                            }

                            if ((s8)arg0[0x40] >= 3) {
                                GSlogWrite((const char *)lbl_80271700, lbl_80315678);
                            } else {
                                *(u32 *)(arg0 + 0x34 + (s8)arg0[0x40] * 4) = (u32)ip + 2;
                                arg0[0x40]++;
                            }
                        }
                    }
                }

                if (pass == 0) {
                    if (arg0[0x4B] == 2) {
                        *(u32 *)(arg0 + 0x54) = *(u32 *)(arg0 + 0x30);
                        break;
                    }
                } else if (arg0[0x4B] == 0) {
                    break;
                }
                continue;
            }

            fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(arg0, code, NULL, NULL);
            if (fontInfo != NULL) {
                glyphWidth = ((u8 *)fontInfo)[2];
            } else {
                glyphWidth = arg0[0x22];
            }

            if (pass == 0) {
                count0++;
                width0 = (s16)((f32)width0 + ((f32)(s16)glyphWidth * *(f32 *)(arg0 + 0x60)) + lbl_8047CD30);
            } else {
                count1++;
                width1 = (s16)((f32)width1 + (lbl_8047CD34 * ((f32)(s16)glyphWidth * *(f32 *)(arg0 + 0x60))) + lbl_8047CD30);
            }
        }
    }

    arg0[0x5A] = count0;
    arg0[0x5B] = count1;
    *(f32 *)(arg0 + 0x4C) = *(f32 *)(arg0 + 0x0C) + (f32)((s32)(((width0 - width1) >> 0x1F) + (width0 - width1)) >> 1);
    *(f32 *)(arg0 + 0x5C) = lbl_8047CD34 * ((f32)arg0[0x22] * *(f32 *)(arg0 + 0x60));
    *(f32 *)(arg0 + 0x50) = -((lbl_8047CD38 * (f32)arg0[0x23]) - *(f32 *)(arg0 + 0x10));

    *(u32 *)(arg0 + 0x30) = (u32)savedIp;
    arg0[0x40] = savedDepth;
    *(u32 *)(arg0 + 0x34) = (u32)resume0;
    *(u32 *)(arg0 + 0x38) = (u32)resume1;
    *(u32 *)(arg0 + 0x3C) = (u32)resume2;
    arg0[0x45] = savedMode;
    arg0[1] = savedFlag;
}
#pragma pop

/* 0x800FAEF8 | 0x544 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 fn_800FAEF8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, farg0, farg1, farg2, farg3, farg4, farg5, farg6, farg7, arg_sp8)
    s32 arg0;
    s32 arg1;
    s32 arg2;
    s32 arg3;
    s32 arg4;
    s32 arg5;
    s32 arg6;
    s32 arg7;
    f64 farg0;
    f64 farg1;
    f64 farg2;
    f64 farg3;
    f64 farg4;
    f64 farg5;
    f64 farg6;
    f64 farg7;
    M2C_UNK arg_sp8;
{
    /* TODO: replace compile-only placeholder with real C decompilation. */
    return 0;
}
#pragma pop

/* 0x800FB43C | 0x244 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 std001_messageFormatLookup(arg0, arg1, arg2)
    s32 arg0;
    s32 arg1;
    u32 arg2;
{
    u8 *sp8;
    f64 var_f2;
    s32 temp_cr0_eq;
    s32 temp_r10;
    s32 var_r7_2;
    u16 temp_r0_2;
    u16 var_ctr;
    u16 var_r6;
    u32 temp_r0;
    u32 temp_r3;
    u32 temp_r5;
    u32 var_r7;
    u8 temp_r3_2;
    u8 temp_r3_4;
    u8 *temp_r3_3;
    u8 *temp_r8;
    u8 *var_r27;
    u8 *var_r9;

    if (arg2 == 0) {
        var_r27 = 0;
    } else {
        temp_r5 = arg2 & 0xFFFFF;
        var_r9 = (void*)(M2C_FIELD(lbl_80478B08, void **, 8));
loop_14:
        if (var_r9 == 0) {
            var_r27 = 0;
        } else if ((u16) M2C_FIELD(var_r9, u16 *, 0) == (u32) (arg2 >> 0x14U)) {
            var_r6 = M2C_FIELD(var_r9, u16 *, 4);
            temp_r8 = (void*)(var_r9 + 0x10);
            var_r7 = 0U;
loop_12:
            if (var_r7 >= var_r6) {
                goto block_13;
            }
            temp_r3 = (u32) (var_r7 + var_r6) >> 1U;
            temp_r10 = temp_r3 * 8;
            temp_r0 = *(temp_r8 + temp_r10);
            if (temp_r0 == temp_r5) {
                if (&sp8 != 0) {
                    sp8 = (void*)(var_r9);
                }
                var_r27 = (void*)(var_r9 + M2C_FIELD((temp_r8 + temp_r10), s32 *, 4));
            } else {
                if (temp_r0 < temp_r5) {
                    var_r7 = temp_r3 + 1;
                } else {
                    var_r6 = (u16) temp_r3;
                }
                goto loop_12;
            }
        } else {
block_13:
            var_r9 = (void*)(M2C_FIELD(var_r9, void **, 8));
            goto loop_14;
        }
    }
    if (var_r27 == 0) {
        return -1;
    }
    memset((u8 *)&lbl_80402418, 0, 0x68);
    var_f2 = lbl_8047CD10;
    var_r7_2 = 0;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0) = 1;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x60) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x64) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = -1;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x28) = var_r27;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x2C) = var_r27;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x30) = var_r27;
    temp_r3_2 = M2C_FIELD(sp8, u8 *, 3);
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x20) = (s16) temp_r3_2;
    M2C_FIELD((u8 *)&lbl_80402418, u32 *, 0x1C) = arg2;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 4) = (f32) arg0;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 8) = (f32) arg1;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x18) = 0;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x1A) = 0;
    M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44) = 3U;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = -1;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 2) = 1;
    temp_r0_2 = M2C_FIELD(lbl_80478B08, u16 *, 4);
    var_ctr = temp_r0_2;
    if ((s32) temp_r0_2 > 0) {
loop_19:
        temp_r3_3 = (void*)(M2C_FIELD(lbl_80478B08, s32 *, 0x24) + var_r7_2);
        if ((u16) M2C_FIELD(temp_r3_3, u16 *, 0) == (u16) temp_r3_2) {
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x22) = (u8) M2C_FIELD(temp_r3_3, u8 *, 2);
            temp_r3_4 = M2C_FIELD(temp_r3_3, u8 *, 3);
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x23) = temp_r3_4;
            if ((u16) temp_r3_2 == 0) {
                M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 0xB;
            } else {
                temp_cr0_eq = (u16) temp_r3_2 == 1;
                if ((temp_cr0_eq != 0) || (temp_cr0_eq != 0)) {
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 6;
                } else {
                    var_f2 = lbl_8047CD28;
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = (s8) ((lbl_8047CD20 * (f64) temp_r3_4) + lbl_8047CD18);
                }
            }
        } else {
            var_r7_2 += 8;
            var_ctr -= 1;
            if (var_ctr != 0) {
                goto loop_19;
            }
        }
    }
    return msgTaskDraw((u8 *)&lbl_80402418, M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44), 0, 0, var_f2);
}
#pragma pop

/* 0x800FB680 | 0x248 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 fn_800FB680(arg0, arg1, arg2, arg3)
    s32 arg0;
    s32 arg1;
    s32 arg2;
    u32 arg3;
{
    u8 *sp8;
    f64 var_f2;
    s32 temp_cr0_eq;
    s32 temp_r10;
    s32 var_r7_2;
    u16 temp_r0_2;
    u16 var_ctr;
    u16 var_r6;
    u32 temp_r0;
    u32 temp_r3;
    u32 temp_r5;
    u32 var_r7;
    u8 temp_r3_2;
    u8 temp_r3_4;
    u8 *temp_r3_3;
    u8 *temp_r8;
    u8 *var_r26;
    u8 *var_r9;

    if (arg3 == 0) {
        var_r26 = 0;
    } else {
        temp_r5 = arg3 & 0xFFFFF;
        var_r9 = (void*)(M2C_FIELD(lbl_80478B08, void **, 8));
loop_14:
        if (var_r9 == 0) {
            var_r26 = 0;
        } else if ((u16) M2C_FIELD(var_r9, u16 *, 0) == (u32) (arg3 >> 0x14U)) {
            var_r6 = M2C_FIELD(var_r9, u16 *, 4);
            temp_r8 = (void*)(var_r9 + 0x10);
            var_r7 = 0U;
loop_12:
            if (var_r7 >= var_r6) {
                goto block_13;
            }
            temp_r3 = (u32) (var_r7 + var_r6) >> 1U;
            temp_r10 = temp_r3 * 8;
            temp_r0 = *(temp_r8 + temp_r10);
            if (temp_r0 == temp_r5) {
                if (&sp8 != 0) {
                    sp8 = (void*)(var_r9);
                }
                var_r26 = (void*)(var_r9 + M2C_FIELD((temp_r8 + temp_r10), s32 *, 4));
            } else {
                if (temp_r0 < temp_r5) {
                    var_r7 = temp_r3 + 1;
                } else {
                    var_r6 = (u16) temp_r3;
                }
                goto loop_12;
            }
        } else {
block_13:
            var_r9 = (void*)(M2C_FIELD(var_r9, void **, 8));
            goto loop_14;
        }
    }
    if (var_r26 == 0) {
        return -1;
    }
    memset((u8 *)&lbl_80402418, 0, 0x68);
    var_f2 = lbl_8047CD10;
    var_r7_2 = 0;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0) = 1;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x60) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x64) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = -1;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x28) = var_r26;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x2C) = var_r26;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x30) = var_r26;
    temp_r3_2 = M2C_FIELD(sp8, u8 *, 3);
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x20) = (s16) temp_r3_2;
    M2C_FIELD((u8 *)&lbl_80402418, u32 *, 0x1C) = arg3;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 4) = (f32) arg0;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 8) = (f32) arg1;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x18) = 0;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x1A) = 0;
    M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44) = 3U;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = arg2;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 2) = 1;
    temp_r0_2 = M2C_FIELD(lbl_80478B08, u16 *, 4);
    var_ctr = temp_r0_2;
    if ((s32) temp_r0_2 > 0) {
loop_19:
        temp_r3_3 = (void*)(M2C_FIELD(lbl_80478B08, s32 *, 0x24) + var_r7_2);
        if ((u16) M2C_FIELD(temp_r3_3, u16 *, 0) == (u16) temp_r3_2) {
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x22) = (u8) M2C_FIELD(temp_r3_3, u8 *, 2);
            temp_r3_4 = M2C_FIELD(temp_r3_3, u8 *, 3);
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x23) = temp_r3_4;
            if ((u16) temp_r3_2 == 0) {
                M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 0xB;
            } else {
                temp_cr0_eq = (u16) temp_r3_2 == 1;
                if ((temp_cr0_eq != 0) || (temp_cr0_eq != 0)) {
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 6;
                } else {
                    var_f2 = lbl_8047CD28;
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = (s8) ((lbl_8047CD20 * (f64) temp_r3_4) + lbl_8047CD18);
                }
            }
        } else {
            var_r7_2 += 8;
            var_ctr -= 1;
            if (var_ctr != 0) {
                goto loop_19;
            }
        }
    }
    return msgTaskDraw((u8 *)&lbl_80402418, M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44), 0, 0, var_f2);
}
#pragma pop

/* 0x800FB8C8 | 0x26C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 fn_800FB8C8(arg0, arg1, arg2, arg3, arg4, arg5)
    s32 arg0;
    s32 arg1;
    s16 arg2;
    s16 arg3;
    s32 arg4;
    u32 arg5;
{
    u8 *sp8;
    f64 var_f2;
    s32 temp_cr0_eq;
    s32 temp_r10;
    s32 temp_r25;
    s32 var_r7_2;
    u16 temp_r0_2;
    u16 var_ctr;
    u16 var_r6;
    u32 temp_r0;
    u32 temp_r3;
    u32 temp_r5;
    u32 var_r7;
    u8 temp_r3_2;
    u8 temp_r3_4;
    u8 *temp_r3_3;
    u8 *temp_r8;
    u8 *var_r24;
    u8 *var_r9;

    temp_r25 = arg0 + (arg2 - (s16) (GSmsgGetRect(arg5) >> 0x10U));
    if (arg5 == 0) {
        var_r24 = 0;
    } else {
        temp_r5 = arg5 & 0xFFFFF;
        var_r9 = (void*)(M2C_FIELD(lbl_80478B08, void **, 8));
loop_14:
        if (var_r9 == 0) {
            var_r24 = 0;
        } else if ((u16) M2C_FIELD(var_r9, u16 *, 0) == (u32) (arg5 >> 0x14U)) {
            var_r6 = M2C_FIELD(var_r9, u16 *, 4);
            temp_r8 = (void*)(var_r9 + 0x10);
            var_r7 = 0U;
loop_12:
            if (var_r7 >= var_r6) {
                goto block_13;
            }
            temp_r3 = (u32) (var_r7 + var_r6) >> 1U;
            temp_r10 = temp_r3 * 8;
            temp_r0 = *(temp_r8 + temp_r10);
            if (temp_r0 == temp_r5) {
                if (&sp8 != 0) {
                    sp8 = (void*)(var_r9);
                }
                var_r24 = (void*)(var_r9 + M2C_FIELD((temp_r8 + temp_r10), s32 *, 4));
            } else {
                if (temp_r0 < temp_r5) {
                    var_r7 = temp_r3 + 1;
                } else {
                    var_r6 = (u16) temp_r3;
                }
                goto loop_12;
            }
        } else {
block_13:
            var_r9 = (void*)(M2C_FIELD(var_r9, void **, 8));
            goto loop_14;
        }
    }
    if (var_r24 == 0) {
        return -1;
    }
    memset((u8 *)&lbl_80402418, 0, 0x68);
    var_f2 = lbl_8047CD10;
    var_r7_2 = 0;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0) = 1;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x60) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x64) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = -1;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x28) = var_r24;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x2C) = var_r24;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x30) = var_r24;
    temp_r3_2 = M2C_FIELD(sp8, u8 *, 3);
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x20) = (s16) temp_r3_2;
    M2C_FIELD((u8 *)&lbl_80402418, u32 *, 0x1C) = arg5;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 4) = (f32) temp_r25;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 8) = (f32) arg1;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x18) = arg2;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x1A) = arg3;
    M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44) = 3U;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = arg4;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 2) = 1;
    temp_r0_2 = M2C_FIELD(lbl_80478B08, u16 *, 4);
    var_ctr = temp_r0_2;
    if ((s32) temp_r0_2 > 0) {
loop_19:
        temp_r3_3 = (void*)(M2C_FIELD(lbl_80478B08, s32 *, 0x24) + var_r7_2);
        if ((u16) M2C_FIELD(temp_r3_3, u16 *, 0) == (u16) temp_r3_2) {
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x22) = (u8) M2C_FIELD(temp_r3_3, u8 *, 2);
            temp_r3_4 = M2C_FIELD(temp_r3_3, u8 *, 3);
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x23) = temp_r3_4;
            if ((u16) temp_r3_2 == 0) {
                M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 0xB;
            } else {
                temp_cr0_eq = (u16) temp_r3_2 == 1;
                if ((temp_cr0_eq != 0) || (temp_cr0_eq != 0)) {
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 6;
                } else {
                    var_f2 = lbl_8047CD28;
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = (s8) ((lbl_8047CD20 * (f64) temp_r3_4) + lbl_8047CD18);
                }
            }
        } else {
            var_r7_2 += 8;
            var_ctr -= 1;
            if (var_ctr != 0) {
                goto loop_19;
            }
        }
    }
    return msgTaskDraw((u8 *)&lbl_80402418, M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44), 0, 0, var_f2);
}
#pragma pop

/* 0x800FBB34 | 0x254 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 fn_800FBB34(arg0, arg1, arg2, arg3, arg4, arg5)
    s32 arg0;
    s32 arg1;
    s16 arg2;
    s16 arg3;
    s32 arg4;
    u32 arg5;
{
    u8 *sp8;
    f64 var_f2;
    s32 temp_cr0_eq;
    s32 temp_r10;
    s32 var_r7_2;
    u16 temp_r0_2;
    u16 var_ctr;
    u16 var_r6;
    u32 temp_r0;
    u32 temp_r3;
    u32 temp_r5;
    u32 var_r7;
    u8 temp_r3_2;
    u8 temp_r3_4;
    u8 *temp_r3_3;
    u8 *temp_r8;
    u8 *var_r24;
    u8 *var_r9;

    if (arg5 == 0) {
        var_r24 = 0;
    } else {
        temp_r5 = arg5 & 0xFFFFF;
        var_r9 = (void*)(M2C_FIELD(lbl_80478B08, void **, 8));
loop_14:
        if (var_r9 == 0) {
            var_r24 = 0;
        } else if ((u16) M2C_FIELD(var_r9, u16 *, 0) == (u32) (arg5 >> 0x14U)) {
            var_r6 = M2C_FIELD(var_r9, u16 *, 4);
            temp_r8 = (void*)(var_r9 + 0x10);
            var_r7 = 0U;
loop_12:
            if (var_r7 >= var_r6) {
                goto block_13;
            }
            temp_r3 = (u32) (var_r7 + var_r6) >> 1U;
            temp_r10 = temp_r3 * 8;
            temp_r0 = *(temp_r8 + temp_r10);
            if (temp_r0 == temp_r5) {
                if (&sp8 != 0) {
                    sp8 = (void*)(var_r9);
                }
                var_r24 = (void*)(var_r9 + M2C_FIELD((temp_r8 + temp_r10), s32 *, 4));
            } else {
                if (temp_r0 < temp_r5) {
                    var_r7 = temp_r3 + 1;
                } else {
                    var_r6 = (u16) temp_r3;
                }
                goto loop_12;
            }
        } else {
block_13:
            var_r9 = (void*)(M2C_FIELD(var_r9, void **, 8));
            goto loop_14;
        }
    }
    if (var_r24 == 0) {
        return -1;
    }
    memset((u8 *)&lbl_80402418, 0, 0x68);
    var_f2 = lbl_8047CD10;
    var_r7_2 = 0;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0) = 1;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x60) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 0x64) = (f32) lbl_8047CD08;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = -1;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x28) = var_r24;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x2C) = var_r24;
    M2C_FIELD((u8 *)&lbl_80402418, void **, 0x30) = var_r24;
    temp_r3_2 = M2C_FIELD(sp8, u8 *, 3);
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x20) = (s16) temp_r3_2;
    M2C_FIELD((u8 *)&lbl_80402418, u32 *, 0x1C) = arg5;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 4) = (f32) arg0;
    M2C_FIELD((u8 *)&lbl_80402418, f32 *, 8) = (f32) arg1;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x18) = arg2;
    M2C_FIELD((u8 *)&lbl_80402418, s16 *, 0x1A) = arg3;
    M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44) = 3U;
    M2C_FIELD((u8 *)&lbl_80402418, s32 *, 0x24) = arg4;
    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 2) = 1;
    temp_r0_2 = M2C_FIELD(lbl_80478B08, u16 *, 4);
    var_ctr = temp_r0_2;
    if ((s32) temp_r0_2 > 0) {
loop_19:
        temp_r3_3 = (void*)(M2C_FIELD(lbl_80478B08, s32 *, 0x24) + var_r7_2);
        if ((u16) M2C_FIELD(temp_r3_3, u16 *, 0) == (u16) temp_r3_2) {
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x22) = (u8) M2C_FIELD(temp_r3_3, u8 *, 2);
            temp_r3_4 = M2C_FIELD(temp_r3_3, u8 *, 3);
            M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x23) = temp_r3_4;
            if ((u16) temp_r3_2 == 0) {
                M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 0xB;
            } else {
                temp_cr0_eq = (u16) temp_r3_2 == 1;
                if ((temp_cr0_eq != 0) || (temp_cr0_eq != 0)) {
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = 6;
                } else {
                    var_f2 = lbl_8047CD28;
                    M2C_FIELD((u8 *)&lbl_80402418, s8 *, 0x42) = (s8) ((lbl_8047CD20 * (f64) temp_r3_4) + lbl_8047CD18);
                }
            }
        } else {
            var_r7_2 += 8;
            var_ctr -= 1;
            if (var_ctr != 0) {
                goto loop_19;
            }
        }
    }
    return msgTaskDraw((u8 *)&lbl_80402418, M2C_FIELD((u8 *)&lbl_80402418, u8 *, 0x44), 0, 0, var_f2);
}
#pragma pop

/* 0x800FBD88 | 0xF4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSmsgFree(u32 key) {
    u8* head;
    u32 count;
    u8* entry;
    u8 i;
    u8 type;
    u32 r3;

    head = (u8*)lbl_80478B08;
    count = *(u16*)head;
    entry = NULL;
    for (i = 0; (u32)(i & 0xFF) < count; i++) {
        u8* e = (u8*)*(u32*)(head + 0x20) + (u32)(i & 0xFF) * 0x68;
        if (*(u8*)(e + 0x0) != 0 && *(u32*)(e + 0x1C) == key) {
            entry = e;
            break;
        }
    }
    if (entry == NULL) return;
    type = *(u8*)(entry + 0x3);
    r3 = 0;
    if (type == 1) r3 = 0x57;
    else if (type == 2) r3 = 0x58;
    else if (type == 3) r3 = 0x59;
    else if (type == 4) r3 = 0x497;
    else if (type == 5) r3 = 0x498;
    if (r3 != 0) soundStopSE(r3);
    *(u8*)(entry + 0x0) = 0;
}
#pragma pop

/* 0x800FBE7C | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
s32 winMsgDrawMessage(u32 key, u32 r4arg) {
    u8* head;
    u32 count;
    u8* entry;
    u8 i;

    head = (u8*)lbl_80478B08;
    count = *(u16*)head;
    entry = NULL;
    for (i = 0; (u32)(i & 0xFF) < count; i++) {
        u8* e = (u8*)*(u32*)(head + 0x20) + (u32)(i & 0xFF) * 0x68;
        if (*(u8*)(e + 0x0) != 0 && *(u32*)(e + 0x1C) == key) {
            entry = e;
            break;
        }
    }
    if (entry == NULL) return -1;
    return msgTaskDraw(entry, *(u8*)(entry + 0x44), r4arg);
}
#pragma pop

/* 0x800FBF10 | 0x64 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void GSmsgDaemon(void) {
    extern u32 lbl_80478B08;
    extern void GStextureUnlockImage(u32 val);
    u8* ptr;
    s8 idx;
    ptr = (u8*)lbl_80478B08;
    idx = (s8)ptr[0x1d];
    ptr += (s32)idx * 4;
    GStextureUnlockImage(*(u32*)(ptr + 0xc));
    *(u16*)((u8*)lbl_80478B08 + 0x18) = 2;
    *(u16*)((u8*)lbl_80478B08 + 0x1a) = 1;
    ptr = (u8*)lbl_80478B08;
    idx = (s8)(ptr[0x1d] ^ 1);
    ptr[0x1d] = (u8)idx;
}
#pragma pop

/* 0x800FBF74 | 0x25C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 GSmsgExec(arg0, arg1, arg2)
    u32 arg0;
    s8 arg1;
    s8 arg2;
{
    u8 *mgr;
    u8 *work;
    u8 *bank;
    u8 *text;
    u8 *font;
    u32 wanted;
    u32 mid;
    u16 low;
    u16 high;
    u16 fontId;
    u16 i;

    mgr = (u8 *)lbl_80478B08;
    work = NULL;
    for (i = 0; i < *(u16 *)mgr; i++) {
        work = (u8 *)*(u32 *)(mgr + 0x20) + i * 0x68;
        if (work[0] == 0) {
            break;
        }
    }
    if (i == *(u16 *)mgr) {
        GSlogWrite((const char *)lbl_80271730, arg0);
        return -1;
    }

    bank = NULL;
    text = NULL;
    if (arg0 != 0) {
        wanted = arg0 & 0xFFFFF;
        bank = (u8 *)*(u32 *)(mgr + 0x08);
        while (bank != NULL) {
            if (*(u16 *)bank == (u16)(arg0 >> 0x14)) {
                low = 0;
                high = *(u16 *)(bank + 0x04);
                while (low < high) {
                    mid = ((u32)low + (u32)high) >> 1;
                    font = bank + 0x10 + mid * 8;
                    if (*(u32 *)font == wanted) {
                        text = bank + *(s32 *)(font + 4);
                        break;
                    }
                    if (*(u32 *)font < wanted) {
                        low = (u16)(mid + 1);
                    } else {
                        high = (u16)mid;
                    }
                }
                if (text != NULL) {
                    break;
                }
            }
            bank = (u8 *)*(u32 *)(bank + 0x08);
        }
    }
    if (text == NULL) {
        GSlogWrite((const char *)lbl_80271754, arg0);
        return -1;
    }

    memset(work, 0, 0x68);
    work[0] = 1;
    *(f32 *)(work + 0x60) = lbl_8047CD08;
    *(f32 *)(work + 0x64) = lbl_8047CD08;
    *(s32 *)(work + 0x24) = -1;
    *(u32 *)(work + 0x28) = (u32)text;
    *(u32 *)(work + 0x2C) = (u32)text;
    *(u32 *)(work + 0x30) = (u32)text;
    fontId = bank[3];
    *(u16 *)(work + 0x20) = fontId;
    *(u32 *)(work + 0x1C) = arg0;
    *(s8 *)(work + 0x44) = arg1;
    *(s8 *)(work + 0x03) = arg2;

    for (i = 0; i < *(u16 *)(mgr + 0x04); i++) {
        font = (u8 *)*(u32 *)(mgr + 0x24) + i * 8;
        if (*(u16 *)font == fontId) {
            work[0x22] = font[2];
            work[0x23] = font[3];
            if (fontId == 0) {
                *(s8 *)(work + 0x42) = 0xB;
            } else if (fontId == 1) {
                *(s8 *)(work + 0x42) = 6;
            } else {
                *(s8 *)(work + 0x42) = (s8)((lbl_8047CD20 * (f64)font[3]) + lbl_8047CD18);
            }
            break;
        }
    }
    return 0;
}
#pragma pop

/* 0x800FC1D0 | 0x74 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
s32 GSmsgClose(u32* item) {
    u32* head;
    u32* p;

    head = (u32*)lbl_80478B08;
    if ((u32*)head[2] == NULL) return -1;
    p = (u32*)head[2];
    while (p != NULL) {
        if (p == item) {
            if (p[3] != 0) ((u32*)p[3])[2] = p[2];
            else head[2] = p[2];
            if (p[2] != 0) ((u32*)p[2])[3] = p[3];
            break;
        }
        p = (u32*)p[2];
    }
    return 0;
}
#pragma pop

/* 0x800FC244 | 0x60 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSmsgOpen(u32* item) {
    u32* head;
    u32* p;

    head = (u32*)lbl_80478B08;
    p = (u32*)head[2];
    if (p == NULL) {
        head[2] = (u32)item;
        item[2] = 0;
        item[3] = 0;
        return;
    }
    while (1) {
        if (p == item) return;
        if (p[2] == 0) {
            p[2] = (u32)item;
            item[2] = 0;
            item[3] = (u32)p;
            return;
        }
        p = (u32*)p[2];
    }
}
#pragma pop

/* 0x800FC2A4 | 0x4 | void_stub */
void fn_800FC2A4(void) {
}

/* 0x800FC2A8 | 0xF4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 4
#pragma scheduling on
u32 fn_800FC2A8(void* ptr) {
    u8* p;
    register s32 offset;
    register u8* head;
    register s32 count;
    register u8* entry;
    register u8* nodePrev;
    register u8* node;
    register s32 idx;

    p = (u8*)ptr;
tail:
    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    idx = 0;
    offset = 0;
    for (; idx < count; offset += 8, idx++) {
        entry = (u8*)*(u32*)(head + 0x24) + offset;
        if (*(u32*)(entry + 0x4) != 0) {
            if (*(u16*)entry == *(u16*)p) break;
        }
    }
    if (idx != count) {
        node = (u8*)*(u32*)(entry + 0x4);
        while (node != NULL) {
            if (node == p + 8) {
                nodePrev = (u8*)*(u32*)(node + 0xC);
                if (nodePrev == NULL && *(u32*)(node + 0x8) == 0) {
                    *(u16*)entry = 0xFFFF;
                    *(u32*)(entry + 0x4) = 0;
                } else {
                    if (nodePrev != NULL) {
                        *(u32*)(nodePrev + 0x8) = *(u32*)(node + 0x8);
                    } else {
                        *(u32*)(entry + 0x4) = *(u32*)(node + 0x8);
                    }
                    if (*(u32*)(node + 0x8) != 0) {
                        *(u32*)(*(u32*)(node + 0x8) + 0xC) = *(u32*)(node + 0xC);
                    }
                }
                break;
            }
            node = (u8*)*(u32*)(node + 0x8);
        }
    }
    if (*(u32*)(p + 0x4) != 0) {
        p += *(u32*)(p + 0x4);
        goto tail;
    }
    return 0;
}
#pragma pop

/* 0x800FC39C | 0x17C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void* GSmsgFontOpen(ptr)
    void* ptr;
{
    u8* p;
    u8* orig;
    u8* head;
    u8* entry;
    u8* node;
    u8* node2;
    u16 key;
    u16 count;
    u16 idx;
    u32 offset;

    orig = (u8*)ptr;
    p = orig;
loop:
    key = *(u16*)p;
    if (key == 0xFFFF) return NULL;

    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    idx = 0;
    offset = 0;
    entry = NULL;
    while (count > 0) {
        entry = (u8*)*(u32*)(head + 0x24) + offset;
        if (*(u32*)(entry + 0x4) != 0) {
            if (*(u16*)entry == key) break;
        }
        offset += 8;
        idx++;
        count--;
        entry = NULL;
    }
    if (entry == NULL) {
        /* No matching occupied slot. Find free slot for new key. */
        count = *(u16*)(head + 0x4);
        idx = 0;
        offset = 0;
        entry = NULL;
        while (count > 0) {
            entry = (u8*)*(u32*)(head + 0x24) + offset;
            if (*(u32*)(entry + 0x4) == 0) {
                /* Insert into free slot */
                *(u32*)(entry + 0x0) = *(u32*)(p + 0x0);
                *(u32*)(entry + 0x4) = (u32)(p + 0x8);
                *(u32*)(p + 0x8 + 0x8) = 0;
                *(u32*)(p + 0x8 + 0xC) = 0;
                break;
            }
            offset += 8;
            idx++;
            count--;
            entry = NULL;
        }
        if (entry == NULL) {
            GSlogWrite((const char*)lbl_8027177C, *(u16*)p);
        }
    } else {
        /* Found occupied slot with matching key; insert node into list */
        node = p + 8;
        node2 = (u8*)*(u32*)(entry + 0x4);
        while (1) {
            if (node2 == node) return NULL;
            if (*(u32*)(node2 + 0x8) == 0) {
                *(u32*)(node2 + 0x8) = (u32)node;
                *(u32*)(node + 0x8) = 0;
                *(u32*)(node + 0xC) = (u32)node2;
                break;
            }
            node2 = (u8*)*(u32*)(node2 + 0x8);
        }
    }
    if (*(u32*)(p + 0x4) != 0) {
        p += *(u32*)(p + 0x4);
        goto loop;
    }
    return orig;
}
#pragma pop

/* 0x800FC518 | 0x10 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
s32 GSmsgSetCtrlFunc(u32 val) {
    *(u32*)((u8*)lbl_80478B08 + 0x28) = val;
    return 0;
}
#pragma pop

/* 0x800FC528 | 0x2B8 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 GSmsgInit(arg0, arg1)
    u16 arg0;
    u16 arg1;
{
    u8 *mgr;
    u8 *work;
    u8 *slot;
    u16 i;
    u16 handle;

    memset((u8 *)&lbl_804024E8, 0, 0x2C);
    mgr = (u8 *)lbl_80478B08;
    handle = GSmemAllocRaw((u32)arg0 * 0x68);
    *(u16 *)(mgr + 0x02) = handle;
    if (handle == 0) {
        GSlogWrite((const char *)lbl_802717B4);
        return -1;
    }
    *(u32 *)(mgr + 0x20) = (u32)GSmemGetPtr(handle);
    handle = GSmemAllocRaw((u32)arg1 * 8);
    *(u16 *)(mgr + 0x06) = handle;
    if (handle == 0) {
        GSlogWrite((const char *)lbl_802717B4);
        return -1;
    }
    *(u32 *)(mgr + 0x24) = (u32)GSmemGetPtr(handle);
    for (i = 0; i < arg0; i++) {
        work = (u8 *)*(u32 *)(mgr + 0x20) + i * 0x68;
        memset(work, 0, 0x68);
        *(f32 *)(work + 0x60) = lbl_8047CD08;
        *(f32 *)(work + 0x64) = lbl_8047CD08;
    }
    *(u16 *)(mgr + 0x00) = arg0;
    for (i = 0; i < arg1; i++) {
        slot = (u8 *)*(u32 *)(mgr + 0x24) + i * 8;
        *(u16 *)slot = 0xFFFF;
        *(u32 *)(slot + 0x04) = 0;
    }
    *(u16 *)(mgr + 0x04) = arg1;
    *(u32 *)(mgr + 0x0C) = (u32)GStextureCreate(0x200, 0x200, 0x40, 0, 0);
    *(u32 *)(mgr + 0x10) = (u32)GStextureCreate(0x200, 0x200, 0x40, 0, 0);
    return 0;
}
#pragma pop

/* 0x800FC7E0 | 0xB68 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 msgTaskDraw(arg0, arg1, arg2, arg3)
    u8 *arg0;
    u8 arg1;
    s32 arg2;
    u8 arg3;
{
    /* TODO: replace compile-only placeholder with real C decompilation. */
    return 0;
}
#pragma pop

/* 0x800FD348 | 0x354 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void fn_800FD348(arg0, farg1)
    u8 *arg0;
    f64 farg1;
{
    void *fontNode;
    u16 *scan;
    u16 code;
    u32 glyphInfo;
    f32 savedX;
    f32 savedY;
    f32 scaleX;
    f32 scaleY;
    f64 spaceBias;
    s32 targetCount;
    u8 drawCount;
    u8 glyphWidth;
    u8 glyphHeight;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    void *fontInfo;

    spaceBias = farg1;
    arg0[0x58]++;
    savedX = *(f32 *)(arg0 + 0x0C);
    savedY = *(f32 *)(arg0 + 0x10);
    *(f32 *)(arg0 + 0x0C) = *(f32 *)(arg0 + 0x4C);
    *(f32 *)(arg0 + 0x10) = *(f32 *)(arg0 + 0x50);
    *(f32 *)(arg0 + 0x60) = *(f32 *)(arg0 + 0x60) * lbl_8047CD34;
    *(f32 *)(arg0 + 0x64) = *(f32 *)(arg0 + 0x64) * lbl_8047CD34;

    targetCount = (s32)(arg0[0x5B] * arg0[0x58]) / (s32)arg0[0x5A];
    drawCount = (u8)targetCount - arg0[0x59];
    scan = *(u16 **)(arg0 + 0x54);

    while (drawCount != 0) {
        code = *scan;
        scan += 2;
        if (code == 0xFFFF) {
            scan++;
            continue;
        }

        if (code == 0x20) {
            spaceBias = lbl_8047CD10;
            *(f32 *)(arg0 + 0x14) = (f32)((arg0[0x22] >> 1) * *(f32 *)(arg0 + 0x60));
        } else {
            fontNode = NULL;
            fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(arg0, code, &fontNode, NULL);
            if (fontInfo == NULL) {
                glyphHeight = arg0[0x23];
                x0 = (s16)*(f32 *)(arg0 + 0x0C);
                y0 = (s16)((s32)*(f32 *)(arg0 + 0x10) + 2);
                scaleX = *(f32 *)(arg0 + 0x60);
                scaleY = *(f32 *)(arg0 + 0x64);
                x1 = (s16)((arg0[0x22] * scaleX) + (f32)x0);
                y1 = (s16)(((f32)glyphHeight * scaleY) + (f32)y0);

                gfxClearFlags(0x80000002, glyphHeight, scaleY, (f32)glyphHeight, (f32)arg0[0x22]);
                gfxSetPrimitive(7);
                gfxSetVtxDesc(lbl_80314E08);
                gfxBegin(2);
                fn_800D61E4(x0, y0);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D61E4(x1, y1);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                gfxEnd();
                gfxSetFlags(0x80000002);
                gfxSetVtxDesc(lbl_80314F98);
                fn_800DC1D4(1);

                spaceBias = lbl_8047CD28;
                *(f32 *)(arg0 + 0x14) = lbl_8047CD30 + (arg0[0x22] * *(f32 *)(arg0 + 0x60));
            } else {
                glyphInfo = *(u32 *)((u8 *)fontInfo + 4);
                glyphWidth = ((u8 *)fontInfo)[2];
                fn_800FD69C(arg0, (u32)fontNode + (*(s32 *)((u8 *)fontNode + 4) + (glyphInfo & 0xFFFFFF)),
                            glyphWidth, ((u8 *)fontInfo)[3], (s8)(glyphInfo >> 0x18));
                spaceBias = lbl_8047CD10;
                *(f32 *)(arg0 + 0x14) = (f32)((s16)glyphWidth * *(f32 *)(arg0 + 0x60));
            }
        }

        drawCount--;
        *(f32 *)(arg0 + 0x0C) += *(f32 *)(arg0 + 0x14);
    }

    *(f32 *)(arg0 + 0x4C) = *(f32 *)(arg0 + 0x0C);
    *(f32 *)(arg0 + 0x50) = *(f32 *)(arg0 + 0x10);
    *(u32 *)(arg0 + 0x54) = (u32)scan;
    arg0[0x59] = (u8)targetCount;
    *(f32 *)(arg0 + 0x0C) = savedX;
    *(f32 *)(arg0 + 0x10) = savedY;
    *(f32 *)(arg0 + 0x60) = *(f32 *)(arg0 + 0x60) * lbl_8047CD30;
    *(f32 *)(arg0 + 0x64) = *(f32 *)(arg0 + 0x64) * lbl_8047CD30;
}
#pragma pop

/* 0x800FD69C | 0x880 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void fn_800FD69C(arg0, arg1, arg2, arg3, arg4)
    u8 *arg0;
    s32 arg1;
    s16 arg2;
    s16 arg3;
    s16 arg4;
{
    /* TODO: replace compile-only placeholder with real C decompilation. */
}
#pragma pop

/* 0x800FDF1C | 0xC8 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
u16 *_msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(arg0, arg1, arg2, arg7)
    u8 *arg0;
    u16 arg1;
    void **arg2;
    u16 *arg7;
{
    s32 temp_cr0_lt;
    s32 var_r11;
    s32 var_r7;
    u16 *temp_r3;
    u16 *var_r10;
    u16 temp_r0;
    u16 temp_r9;
    u16 var_ctr;
    u16 var_r9;
    u32 temp_r10;
    u32 var_r8;
    u8 *var_r6;

    var_r10 = (void*)(arg7);
    var_r11 = 0;
    var_r7 = 0;
    temp_r9 = M2C_FIELD(lbl_80478B08, u16 *, 4);
    var_ctr = temp_r9;
    if ((s32) temp_r9 > 0) {
loop_1:
        var_r10 = (void*)(M2C_FIELD(lbl_80478B08, s32 *, 0x24) + var_r7);
        if ((u16) *var_r10 != (u16) M2C_FIELD(arg0, u16 *, 0x20)) {
            var_r7 += 8;
            var_r11 += 1;
            var_ctr -= 1;
            if (var_ctr != 0) {
                goto loop_1;
            }
        }
    }
    if (var_r11 == (s32) temp_r9) {
        return 0;
    }
    var_r6 = (void*)(M2C_FIELD(var_r10, void **, 4));
loop_16:
    if (var_r6 == 0) {

        return 0;
    }
    var_r9 = M2C_FIELD(var_r6, u16 *, 0);
    var_r8 = 0U;
loop_13:
    temp_cr0_lt = var_r8 < var_r9;
    if (temp_cr0_lt == 0) {
        if (temp_cr0_lt == 0) {
            var_r6 = (void*)(M2C_FIELD(var_r6, void **, 8));
            goto loop_16;
        }
        return 0;
    }
    temp_r10 = (u32) (var_r8 + var_r9) >> 1U;
    temp_r3 = (void*)(var_r6 + 0x10 + (temp_r10 * 8));
    temp_r0 = *temp_r3;
    if (temp_r0 == arg1) {
        if (arg2 != 0) {
            *arg2 = var_r6;
            return temp_r3;
        }
        return temp_r3;
    }
    if (temp_r0 < arg1) {
        var_r8 = temp_r10 + 1;
    } else {
        var_r9 = (u16) temp_r10;
    }
    goto loop_13;
}
#pragma pop

/* 0x800FDFE4 | 0x2C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 4
s32 _msgGetLength__FPCUs(const void* str) {
    s32 r;
    r = _msgGetSize__FPCUs(str);
    return ((u32)(r + 1) >> 1) - 1;
}
#pragma pop

/* 0x800FE010 | 0x34C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
s32 _msgGetSize__FPCUs(arg0)
    u16 *arg0;
{
    u8 *work;
    u16 *ip;
    u16 code;
    u16 control;
    u8 *mgr;
    u8 *table;
    u8 *entry;
    u8 *node;
    u32 flags;
    u32 result;
    u32 mode;
    u32 subKey;
    u16 lo;
    u16 hi;
    u32 mid;
    s32 i;

    if (arg0 == NULL) {
        GSlogWrite((const char *)lbl_802717D4);
        return 0;
    }

    work = (u8 *)&lbl_80402480;
    memset(work, 0, 0x68);
    work[0] = 1;
    work[1] = 1;
    *(f32 *)(work + 0x60) = lbl_8047CD08;
    *(f32 *)(work + 0x64) = lbl_8047CD08;
    *(s32 *)(work + 0x24) = -1;
    *(u32 *)(work + 0x28) = (u32)arg0;
    *(u32 *)(work + 0x2C) = (u32)arg0;
    *(u32 *)(work + 0x30) = (u32)arg0;

    mgr = (u8 *)lbl_80478B08;
    for (i = 0; i < *(u16 *)(mgr + 0x04); i++) {
        entry = (u8 *)*(u32 *)(mgr + 0x24) + i * 8;
        if (*(u16 *)entry == *(u16 *)(work + 0x20)) {
            work[0x22] = entry[2];
            work[0x23] = entry[3];
            if (*(u16 *)(work + 0x20) == 0) {
                *(s8 *)(work + 0x42) = 0xB;
            } else if (*(u16 *)(work + 0x20) == 1) {
                *(s8 *)(work + 0x42) = 6;
            } else {
                *(s8 *)(work + 0x42) = (s8)((lbl_8047CD20 * (f64)entry[3]) + lbl_8047CD18);
            }
            break;
        }
    }

    for (;;) {
        ip = *(u16 **)(work + 0x30);
        code = *ip;
        if (code == 0) {
            if ((s8)work[0x40] == 0) {
                break;
            }
            work[0x40]--;
            *(u32 *)(work + 0x30) = *(u32 *)(work + 0x34 + (s8)work[0x40] * 4);
            continue;
        }

        *(u16 **)(work + 0x30) = ip + 2;
        if (code != 0xFFFF) {
            continue;
        }

        ip = *(u16 **)(work + 0x30);
        control = (u8)*ip;
        *(u16 **)(work + 0x30) = ip + 1;
        table = (u8 *)*(u32 *)(mgr + 0x28);
        if (table == NULL) {
            continue;
        }

        entry = table + control * 8;
        if (work[1] == 0) {
            flags = (entry[0] >> 4) & 1;
        } else {
            flags = (entry[0] >> 3) & 1;
        }
        if (flags == 0 || *(u32 *)(entry + 4) == 0) {
            continue;
        }

        result = ((u32 (*)(u8 *))*(u32 *)(entry + 4))(work);
        mode = (entry[0] >> 6) & 3;
        if (mode == 0 || result == 0) {
            continue;
        }

        if (mode == 1) {
            *(u32 *)(work + 0x30) = result;
        } else if (mode == 2) {
            node = NULL;
            if (result != 0) {
                subKey = result & 0xFFFFF;
                node = (u8 *)*(u32 *)(mgr + 0x08);
                while (node != NULL) {
                    if (*(u16 *)node == (u16)(result >> 0x14)) {
                        lo = 0;
                        hi = *(u16 *)(node + 0x04);
                        while (lo < hi) {
                            mid = ((u32)lo + (u32)hi) >> 1;
                            entry = node + 0x10 + mid * 8;
                            if (*(u32 *)entry == subKey) {
                                *(u32 *)(work + 0x30) = (u32)(node + *(s32 *)(entry + 4));
                                node = NULL;
                                break;
                            }
                            if (*(u32 *)entry < subKey) {
                                lo = (u16)(mid + 1);
                            } else {
                                hi = (u16)mid;
                            }
                        }
                        if (node == NULL) {
                            break;
                        }
                    }
                    node = (u8 *)*(u32 *)(node + 0x08);
                }
            }
        }

        if ((s8)work[0x40] >= 3) {
            GSlogWrite((const char *)lbl_80271700, lbl_80315678);
        } else {
            *(u32 *)(work + 0x34 + (s8)work[0x40] * 4) = (u32)ip + 2;
            work[0x40]++;
        }
    }

    return ((u8 *)*(u32 *)(work + 0x30) - (u8 *)arg0) + 2;
}
#pragma pop

/* 0x800FE35C | 0x30 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
#pragma scheduling off
void fn_800FE35C(void) {
    fn_800D9D68(0, 0, 0x27F, 0x1DF);
}
#pragma scheduling on
#pragma pop

/* 0x800FE38C | 0x148 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
#pragma scheduling on
#pragma peephole off
#pragma push
#pragma optimization_level 3
void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2) {
    s32 ax, ay, bx, by;
    f32 scale_x, scale_y;
    s32 cy2, cx2, cy1, cx1;

    ax = (s32)(*(s16*)&lbl_8047AC70) + x1;
    ay = (s32)(*(s16*)&lbl_8047AC72) + y1;
    bx = ax + x2;
    by = ay + y2;
    scale_x = *(f32*)&lbl_80478B10;
    scale_y = *(f32*)&lbl_80478B14;
    cx1 = (s32)((f32)ax * scale_x);
    cy1 = (s32)((f32)ay * scale_y);
    cx2 = (s32)((f32)bx * scale_x);
    cy2 = (s32)((f32)by * scale_y);
    if (cx1 >= 0x280) cx1 = 0x27F;
    if (cy1 >= 0x1E0) cy1 = 0x1DF;
    if (cx2 >= 0x280) cx2 = 0x27F;
    if (cy2 >= 0x1E0) cy2 = 0x1DF;
    if (cx1 < 0) cx1 = 0;
    if (cy1 < 0) cy1 = 0;
    if (cx2 < 0) cx2 = 0;
    if (cy2 < 0) cy2 = 0;
    fn_800D9D68(cx1, cy1, cx2, cy2);
}
#pragma pop
#pragma pop

/* 0x800FE4D4 | 0x1CC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void spriteSetEnv(void) {
    f32 v0[3];
    f32 v1[3];
    f32 v2[3];
    f32 mtx[12];
    f32 sx;
    f32 sy;
    f32 x;
    f32 y;
    f32 z;
    f32 t;

    sx = lbl_8047CD58 / *(f32*)&lbl_80478B10;
    sy = lbl_8047CD5C / *(f32*)&lbl_80478B14;
    x = sx * lbl_8047CD60;
    y = sy * lbl_8047CD60;
    t = (f32)tan();
    z = y / t;

    set__5GSvecFfff(v0,
                x - (f32)(s32)*(s16*)&lbl_8047AC70,
                y - (f32)(s32)*(s16*)&lbl_8047AC72,
                z);
    set__5GSvecFfff(v1,
                x - (f32)(s32)*(s16*)&lbl_8047AC70,
                y - (f32)(s32)*(s16*)&lbl_8047AC72,
                lbl_8047CD68);
    set__5GSvecFfff(v2, lbl_8047CD68, lbl_8047CD6C, lbl_8047CD68);
    fn_800E0218(mtx, v0, v2, v1);

    fn_800D9BD0(lbl_8047CD70, -(sx / sy), lbl_8047CD74, lbl_8047CD78);
    fn_800D834C();
    fn_800D7FE4(mtx);
    gfxSetBlendMode(1, 6, 7);
    gfxClearFlags(0x80000000);
    gfxSetUpdates(2, 2, 1);
    fn_800DA100(0, 7, 0, 1, 7, 0);
    gfxSetZMode(0, 2, 0);
    gfxSetCullMode(0);
}
#pragma pop

/* 0x800FE6A0 | 0xC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800FE6A0(f32 a, f32 b) {
    *(f32*)&lbl_80478B10 = a;
    *(f32*)&lbl_80478B14 = b;
}
#pragma pop

/* 0x800FE6AC | 0x24 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800FE6AC(s16* outA, s16* outB) {
    if (outA != (void*)0) {
        *outA = *(s16*)&lbl_8047AC70;
    }
    if (outB != (void*)0) {
        *outB = *(s16*)&lbl_8047AC72;
    }
}
#pragma pop

/* 0x800FE6D0 | 0xC */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void fn_800FE6D0(s32 a, s32 b) {
    *(u16*)&lbl_8047AC70 = (u16)a;
    *(u16*)&lbl_8047AC72 = (u16)b;
}
#pragma pop

/* 0x800FE6DC | 0x1C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSgappUnblock(u32 taskId) {
    u32 idx = taskId - 1;
    ((u8*)lbl_8047AC7C)[idx * 0x18 + 0xD] = 0;
}
#pragma pop

/* 0x800FE6F8 | 0x1C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSgappBlock(u32 taskId) {
    u32 idx = taskId - 1;
    ((u8*)lbl_8047AC7C)[idx * 0x18 + 0xD] = 1;
}
#pragma pop

/* 0x800FE714 | 0x8C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
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
#pragma pop

/* 0x800FE7A0 | 0x94 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#pragma optimization_level 2
void GSgappUpdate(void) {
    u32* task;
    u32* next;
    u32 taskId;
    task = (u32*)lbl_8047AC98;
    while (task != NULL) {
        next = (u32*)task[1];
        if (task[2] == 1 && ((u8*)task)[0xD] == 0) {
            lbl_8047AC94 = (u32)task;
            taskId = ((u32)task - lbl_8047AC7C) / 0x18 + 1;
            ((void (*)(u32, void*))task[5])(taskId, (void*)task[4]);
        }
        task = next;
    }
    lbl_8047AC94 = 0;
}
#pragma pop

/* 0x800FE834 | 0x17C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
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
#pragma pop

/* 0x800FE9B0 | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
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
    offset = 8;
    i = 0;
    while (i < total) {
        *(u32*)(lbl_8047AC7C + offset) = 0;
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
#pragma pop

/* 0x800FEA74 | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
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
#pragma pop


/* 0x800F106C | 0x7C: first-party assembly in its own linked unit (evidence:
 * docs/asm_evidence/gs_thread.md). */
#include "src/game/gs_thread_exact_800F106C.c"
