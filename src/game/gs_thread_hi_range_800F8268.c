/**
 * @file gs_thread_hi_range_800F8268.c
 * @brief GSthread (upper half) -- unnamed/unclassified leading block.
 *
 * Address range: 0x800F8268 - 0x800F9318 (7 functions).
 * XD class: UNKNOWN (no clean XD match) -- per the r3 split-spec
 * reconstruction, this block's globals (bss lbl_80401C10 array-scan
 * cluster) are self-contained and not shared with the neighbouring
 * GSres/GSmsg/GSgapp/sprite blocks, so it was not matched to any XD TU
 * by size, count, or call-signature. May be Colosseum-only code.
 *
 * This is one of 6 files split out of the former monolithic
 * game/gs_thread_hi.c (0x800F8268-0x800FF0A0 per config/GC6E01/splits.txt),
 * following the method of commit a46508c9 (battle_waza.c 8-way split).
 *
 * NOTE: the original monolithic file also physically contained a
 * duplicate GSthreadCreate() body (claimed address 0x800F07A8, which is
 * out of range for this unit -- gs_thread.c owns that address and
 * already has its own real definition) plus a large verbatim-duplicate
 * tail (addresses 0x800F0A74-0x800F7108, all of which belong to and
 * already exist correctly in gs_thread.c). Both were dead/out-of-range
 * duplicates (see include/game/gs_thread.h's own "orphan fiction
 * removed" note for the parallel GSTask-family case) and have been
 * dropped rather than carried into any of the 6 new split files.
 *
 * See also: config/GC6E01/splits.txt, include/game/gs_thread.h.
 */
#include "dolphin/types.h"
#include "game/gs_thread.h"

typedef u8 M2C_UNK;
#define M2C_FIELD(base, type, offset) (*(type)((u8*)(base) + (offset)))


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
void* fn_800E24B0(u32 handle);
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

/* 0x800F8268 | 0x1C0 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F8268(void) {
#include "src/game/gs_thread_fn_800F8268.inc"
}
#else
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
#endif
#pragma pop

/* 0x800F8428 | 0x22C */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F8428(void) {
#include "src/game/gs_thread_fn_800F8428.inc"
}
#else
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
#endif
#pragma pop

/* 0x800F8654 | 0x400 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F8654(void) {
#include "src/game/gs_thread_fn_800F8654.inc"
}
#else
void fn_800F8654(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg_sp8)
    u8 *arg0;
    u32 arg1;
    u32 arg2;
    u8 *arg3;
    u8 *arg4;
    f32 *arg5;
    f32 *arg6;
    f32 *arg7;
    f32 *arg_sp8;
{
    f32 temp_f2;
    f32 scale;
    f32 zero;
    f32 var_f1;
    f32 var_f1_2;
    s32 temp_r0;
    s32 var_r0;
    s32 var_r0_2;
    u32 count;
    u8 temp_r30;
    u8 temp_r30_2;
    s32 temp_r3;
    s32 temp_r3_2;
    s32 temp_r3_3;
    s32 temp_r3_4;

    temp_r30 = *arg3;
    if (((s8) temp_r30 < (s32) ((s8)arg1 - 2)) || ((s8) temp_r30 > (s32) ((s8)arg1 + 2)) || (temp_r30_2 = *arg4, (((s8) temp_r30_2 < (s32) ((s8)arg2 - 2)) != 0)) || ((s8) temp_r30_2 > (s32) ((s8)arg2 + 2))) {
        temp_r0 = M2C_FIELD(arg0, s32 *, 0x14);
        if (temp_r0 == 0) {
            *arg5 = ((f32) (s8)arg1 - *arg7) / (f32) M2C_FIELD(arg0, u8 *, 0x10);
            *arg6 = ((f32) (s8)arg2 - *arg_sp8) / (f32) M2C_FIELD(arg0, u8 *, 0x10);
        } else if (temp_r0 == 1) {
            scale = lbl_8047CCD4;
            *arg5 = ((f32) (s8)arg1 - *arg7) / ((f32) M2C_FIELD(arg0, u8 *, 0x10) * scale);
            *arg6 = ((f32) (s8)arg2 - *arg_sp8) / ((f32) M2C_FIELD(arg0, u8 *, 0x10) * scale);
        }
        M2C_FIELD(arg0, u8 *, 0x11) = 0U;
    }
    *arg3 = (u8) arg1;
    *arg4 = (u8) arg2;
    *arg7 += *arg5;
    *arg_sp8 += *arg6;
    if ((s32) M2C_FIELD(arg0, s32 *, 0x14) == 1) {
        M2C_FIELD(arg0, u8 *, 0x11) = (u8) (M2C_FIELD(arg0, u8 *, 0x11) + 1);
        count = M2C_FIELD(arg0, u8 *, 0x11);
        if (count < M2C_FIELD(arg0, u8 *, 0x10)) {
            var_f1 = *arg5;
            scale = lbl_8047CCD8;
            zero = lbl_8047CCD0;
            var_f1 *= scale;
            *arg5 = var_f1;
            var_f1 = *arg6;
            var_f1 *= scale;
            *arg6 = var_f1;
            temp_f2 = *arg5;
            if (temp_f2 > zero) {
                var_f1 = temp_f2;
            } else {
                var_f1 = -temp_f2;
            }
            if (var_f1 < lbl_8047CCDC) {
                if (temp_f2 > zero) {
                    var_r0 = 1;
                } else {
                    var_r0 = -1;
                }
                *arg5 = (f32) var_r0;
            }
            var_f1_2 = *arg6;
            var_f1_2 = var_f1_2 > lbl_8047CCD0 ? var_f1_2 : -var_f1_2;
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
        temp_r3 = (s8)*arg3;
        if (*arg7 < (f32)temp_r3) {
            *arg7 = (f32)temp_r3;
        }
    } else {
        temp_r3_2 = (s8)*arg3;
        if (*arg7 > (f32)temp_r3_2) {
            *arg7 = (f32)temp_r3_2;
        }
    }
    if (*arg6 < lbl_8047CCD0) {
        temp_r3_3 = (s8)*arg4;
        if (*arg_sp8 < (f32)temp_r3_3) {
            *arg_sp8 = (f32)temp_r3_3;
        }
    } else {
        temp_r3_4 = (s8)*arg4;
        if (*arg_sp8 > (f32)temp_r3_4) {
            *arg_sp8 = (f32)temp_r3_4;
        }
    }
}
#endif
#pragma pop

/* 0x800F8A54 | 0x708 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F8A54(void) {
#include "src/game/gs_thread_fn_800F8A54.inc"
}
#else
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

#endif
#pragma pop

/* 0x800F915C | 0xB4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F915C(void) {
#include "src/game/gs_thread_fn_800F915C.inc"
}
#else
#pragma optimization_level 2
void fn_800F915C(u32 key) {
    u32 handle;
    u32 i;
    u8* entry;

    entry = (u8*)lbl_8047AC5C;
    i = lbl_8047AC60;
    while (i-- != 0) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0x8) == key) {
            if (*(u32*)(entry + 0x10) != 0 &&
                (u8)((u32 (*)(u32, u32, u32))(*(u32*)(entry + 0x10)))(
                    *(u32*)(entry + 0x4), *(u32*)(entry + 0x8),
                    *(u32*)(entry + 0xC)) == 0) {
                goto next;
            }
            handle = *(u16*)(entry + 0x0);
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(*(u16*)(entry + 0x0));
                *(u16*)(entry + 0x0) = 0;
            }
            *(u32*)(entry + 0x4) = 0;
        }
next:
        entry += 0x14;
    }
}
#endif
#pragma pop

/* 0x800F9210 | 0xC4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F9210(void) {
#include "src/game/gs_thread_fn_800F9210.inc"
}
#else
#pragma optimization_level 4
void fn_800F9210(u32 key1, u32 key2) {
    u8* entry;
    u32 i;

    entry = (u8*)lbl_8047AC5C;
    for (i = lbl_8047AC60; i > 0; i--) {
        if (*(u32*)(entry + 0x4) != 0 && *(u32*)(entry + 0x8) == key1 && *(u32*)(entry + 0xC) == key2) {
            goto found;
        }
        entry += 0x14;
    }
    entry = NULL;
found:
    if (entry != NULL) {
        if (*(u32*)(entry + 0x10) != 0 &&
            (u8)((u32 (*)(u32, u32, u32))(*(u32*)(entry + 0x10)))(
                *(u32*)(entry + 0x4), *(u32*)(entry + 0x8),
                *(u32*)(entry + 0xC)) == 0) {
            return;
        }
        if (*(u16*)(entry + 0x0) != 0) {
            fn_800E24B0(*(u16*)(entry + 0x0));
            fn_800E209C(*(u16*)(entry + 0x0));
            *(u16*)(entry + 0x0) = 0;
        }
        *(u32*)(entry + 0x4) = 0;
    }
}
#endif
#pragma pop

/* 0x800F92D4 | 0x44 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_800F92D4(void) {
#include "src/game/gs_thread_fn_800F92D4.inc"
}
#else
#pragma optimization_level 4
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
#endif
#pragma pop
