/**
 * @file scene_init.c
 * @brief Scene initialization, transition, and game-start thread management.
 *
 * This module handles the initial game boot sequence after movie playback,
 * including memory card detection, reset-code handling, thread creation for
 * the main game task, fade-in/fade-out timing, and model/floor resource
 * loading during scene transitions.
 *
 * Key behaviors:
 *   - Creates GS threads (via fn_800A19CC) with callback entry points
 *     fn_8003708C and fn_800370E0
 *   - Calls OSGetResetCode to detect warm vs cold boot
 *   - Checks memory card status (VIGetTvFormat area via fn_800AA498)
 *   - Coordinates fade effects through fn_801C41C8 / fn_801C40F0
 *   - Manages floor resource loading through fn_80106D3C / fn_80102568
 *   - Uses timing loops with float accumulators for fade transitions
 *   - Clears memory regions via memset for fresh game state
 *
 * BSS usage:
 *   - lbl_803A3E58 (0xE7 bytes): Scene init state structure
 *   - lbl_803A6498 (0x54 bytes): Sub-scene / card state
 *   - lbl_803A64EC (0x60 bytes): Transition parameter block A
 *   - lbl_803A654C (0x64 bytes): Transition parameter block B
 *   - lbl_803A65B0 (0x60 bytes): Transition parameter block C
 *
 * Address range: 0x8003686C - 0x80039998 (49 functions)
 */

#include "dolphin/types.h"

/* ===== Dolphin OS ===== */
extern u32  OSGetResetCode(void);

/* ===== CRT / libc ===== */
extern void* memset(void* dst, int val, u32 size);
extern void sin();   /* MSL trig (renamed fn_800CE148) — referenced by asm incs */
extern void cos();   /* MSL trig (renamed fn_800CDBE0) — referenced by asm incs */
extern void atan();
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GSlightSetType();
extern void cameraWaitSyncAnime();
extern void menuModelSetMotion();
extern void menuSubOpenYesNo();

/* ===== GS Engine ===== */
extern void  fn_800A19CC(void* ctx, void* callback, void* arg,
                         void* stack, u32 stackSize, u32 priority, u32 flag);
extern void  fn_800A1F94(void* ctx);    /* GS thread start */
extern void  _threadSwitch(void);          /* GSthread yield / step */
extern void  fn_800DD970(const char* fmt, ...);  /* OSReport / GSlog */
extern void  fn_801C41C8(f32 speed, u32 mode);  /* fade set */
extern void  fn_801C40F0(u32 enable);            /* fade enable */
extern u32   fn_80102568(u32 sceneId, u32 a, u32 b);  /* scene/model load */
extern u32   fn_8010264C(u32 sceneId, u32 a);         /* scene/model query */
extern u32   fn_80102510(u32 sceneId);                  /* scene/model unload */
extern s32   fn_801026A4(u32 sceneId, u32 a, u32 b, u32 c,
                         u32 d, u32 e, ...);           /* scene setup */
extern void  fn_80102868(u32 a, u32 b, u32 c);        /* scene positioning */
extern void* fn_80104704(u32 a);         /* scene object query */
extern u8    fn_801045A8(u32 a, u32 b);  /* scene anim check */
extern s32   fn_801043A4(u32 a);         /* scene anim result */
extern void  fn_80106D3C(u32 slot, u32 floorId, u32 a, u32 b);  /* floor load */
extern void* fn_801046B8(void);          /* scene context get */
extern u32   fn_801022B8(u32 a);         /* scene message get */
extern void  fn_800F7F64(u32 pad);       /* input init */
extern u32   fn_800F7BC4(u32 pad);       /* input poll */
extern u32   fn_800AA498(void);          /* VIGetTvFormat / card detect */
extern u32   fn_800A0F58(void);          /* card status */
extern const f32 lbl_8047BA34;     /* timing threshold 2.0f */
extern void  fn_800A0FC8(u32 a);         /* card close */
extern s32   fn_800D37CC(void);          /* GSmem tick */
extern u32   fn_800D3088(void);          /* GSmem query */
extern void  fn_800D37D4(u32 a, u32 b, u32 c, u32 d, u32 e, u16 size);
                                          /* GSmem transfer */
extern u16   fn_800C46B0(f32 a);         /* float-to-u16 convert */
extern void  fn_800E01D0(void* dst, void* src);  /* material copy */
extern void  fn_80177A44(void);          /* gs_scene helper — referenced by asm incs */

/* ===== BSS data ===== */
extern u8    lbl_803A3E58[];   /* Scene init state (0xE7 bytes) */
extern u8    lbl_803A6498[];   /* Sub-scene / card state (0x54 bytes) */
extern u8    lbl_803A64EC[];   /* Transition params A (0x60 bytes) */
extern u8    lbl_803A654C[];   /* Transition params B (0x64 bytes) */
extern u8    lbl_803A65B0[];   /* Transition params C (0x60 bytes) */

/* ===== SDA data ===== */
extern u32   lbl_804788B8;     /* Memory card slot / state flag */
extern u32   lbl_8047A460;     /* Thread completion flag */
extern u32   lbl_8047A464;     /* Thread active flag */

/* ===== SDATA2 (float constants) ===== */
extern const f32 lbl_8047BA30;     /* 0.0f */
extern const f64 lbl_8047BA38;     /* s32->f32 bias */
extern const f64 lbl_8047BA40;     /* u32->f32 bias */
extern const f32 lbl_8047BA48;     /* 10.0f */
extern f32   lbl_8047BA4C;     /* fade speed */
extern f32   lbl_8047BA50;     /* scale factor */

/* ===== Rodata ===== */
extern const u8 lbl_80267050[];  /* Shift-JIS: "読み出しエラー\n" (Read Error) */

/* CW struct copy helpers: triggers register-blast copy pattern */
typedef struct { u32 data[14]; } Tbl14;
typedef struct { u32 data[56]; } Tbl56;
typedef struct { u32 data[78]; } Tbl78;

/*
 * Functions in this translation unit (49 total):
 *
 * fn_8003686C  0x820  Scene main init (OSGetResetCode, thread setup, card detect)
 * fn_8003708C  0x054  Thread callback A (scene init step)
 * fn_800370E0  0x078  Thread callback B (scene init step)
 * fn_80037158  0x004  Stub (blr)
 * fn_8003715C  0x018  Small accessor
 * fn_80037174  0x00C  Small accessor
 * fn_80037180  0x170  BSS lbl_803A6498 state machine
 * fn_800372F0  0x004  Stub
 * fn_800372F4  0x004  Stub
 * fn_800372F8  0x004  Stub
 * fn_800372FC  0x004  Stub
 * fn_80037300  0x0C8  Transition parameter setup
 * fn_800373C8  0x0A0  Transition block A init (lbl_803A64EC, lbl_803A654C)
 * fn_80037468  0x0A0  Transition block A variant
 * fn_80037508  0x0A0  Transition block A variant
 * fn_800375A8  0x0A0  Transition block A variant
 * fn_80037648  0x080  Transition helper
 * fn_800376C8  0x030  Transition helper
 * fn_800376F8  0x0BC  Transition block B (lbl_803A654C)
 * fn_800377B4  0x0BC  Transition block B variant
 * fn_80037870  0x0BC  Transition block B variant
 * fn_8003792C  0x0BC  Transition block B variant
 * fn_800379E8  0x1C8  Transition timing loop
 * fn_80037BB0  0x1C8  Transition timing loop variant
 * fn_80037D78  0x1C8  Transition timing loop variant
 * fn_80037F40  0x1E4  Transition timing loop variant (extended)
 * fn_80038124  0x014  Small accessor
 * fn_80038138  0x038  Helper function
 * fn_80038170  0x0E0  Transition block B accessor (lbl_803A654C)
 * fn_80038250  0x098  Model positioning helper
 * fn_800382E8  0x098  Model positioning helper variant
 * fn_80038380  0x0AC  Model setup
 * fn_8003842C  0x498  Scene transition state machine (calls fn_80038A0C, fn_8003A520)
 * fn_800388C4  0x0CC  Scene completion check
 * fn_80038990  0x070  Scene cleanup
 * fn_80038A00  0x00C  Small accessor
 * fn_80038A0C  0x468  Scene sub-state machine (lbl_803A6498)
 * fn_80038E74  0x190  Transition parameter block C init (lbl_803A65B0)
 * fn_80039004  0x078  Transition block C accessor
 * fn_8003907C  0x0AC  Transition helper
 * fn_80039128  0x370  Transition block C state machine (lbl_803A65B0)
 * fn_80039498  0x0B0  Utility function
 * fn_80039548  0x024  Small accessor
 * fn_8003956C  0x098  Utility function
 * fn_80039604  0x040  Small accessor
 * fn_80039644  0x290  Utility state machine
 * fn_800398D4  0x058  Wrapper (calls fn_80039644)
 * fn_8003992C  0x044  Small function
 * fn_80039970  0x028  Small function
 */

/* ===================================================================
 * DECOMP STUBS
 * These are non-matching placeholder stubs.  Each will be replaced
 * with a matching reimplementation during Phase 2.
 * =================================================================== */


/* forward declarations needed for fn_8003686C */
extern void fn_801069FC();
extern s32  fn_8017B2CC(u32 a);
extern s32 fn_800370E0(u32* arg);

/* 0x8003686C | size: 0x820 */
/* Scene main init - OSGetResetCode handling, thread creation, card detection */
#pragma peephole off
void fn_8003686C(void) {
    u8* ctx;        /* -> r31 */
    s8 anim_result;
    u32 tmp;        /* scratch u32 for fn_801026A4 call */
    u32 tmp2;       /* scratch u32 for path B fn_801026A4 call */
    s8* obj;        /* scratch object pointer */
    s32 is_warm;
    ctx = lbl_803A3E58;
    lbl_804788B8 = (u32)-1;

    /* Warm boot check */
    if ((OSGetResetCode() + 0x80000000u) == 0u)
        is_warm = 1;
    else
        is_warm = 0;

    if (is_warm == 0) {
        /* Cold boot: card / TV detect */
        if (fn_800AA498() == 1) {
            fn_800F7F64(1);
            if (fn_800F7BC4(1) & 0x200u) {
                /* === Path A: cold boot, bit9 set === */
                s8 anim_track;  /* block-local: demotes below the 0x4330 temps -> r28 */
                fn_80106D3C(1, 0x3b50, 1, 1);
                tmp = 0;
                fn_801026A4(0x11, (u32)fn_801046B8(), (u32)&tmp, 0, 0, 0);
                fn_80102868(0x11, 0x2d, 0xbe);

                {
                    f32 acc;
                    acc = lbl_8047BA30;
                    anim_track = -1;
                    while (acc < lbl_8047BA48) {
                        if (fn_801045A8(0x11, 0) == 0) {
                            anim_result = fn_801043A4(0x11);
                            break;
                        }
                        obj = (s8*)fn_80104704(0x11);
                        if (anim_track != obj[0x95]) {
                            acc = lbl_8047BA30;
                            anim_track = obj[0x95];
                        }
                        _threadSwitch();
                        acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                    }
                    if (acc >= lbl_8047BA48) {
                        obj = (s8*)fn_80104704(0x11);
                        anim_result = obj[0x95];
                    }
                }
                fn_80102568(0x11, 0, 1);

                if (anim_result == 0) {
                    {
                        s32 is_w;
                        if ((OSGetResetCode() + 0x80000000u) == 0u)
                            is_w = 1;
                        else
                            is_w = 0;
                        if (is_w == 1) {
                            if (fn_800A0F58() == 0) {
                                lbl_804788B8 = 1;
                            }
                        } else {
                            lbl_804788B8 = 1;
                        }
                    }
                    fn_80106D3C(1, 0x3b51, 1, 1);
                    {
                        f32 acc;
                        acc = lbl_8047BA30;
                        while (acc < lbl_8047BA34) {
                            _threadSwitch();
                            acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                        }
                    }
                } else {
                    {
                        s32 is_w;
                        if ((OSGetResetCode() + 0x80000000u) == 0u)
                            is_w = 1;
                        else
                            is_w = 0;
                        if (is_w == 1) {
                            if (fn_800A0F58() == 1) {
                                lbl_804788B8 = 0;
                            }
                        } else {
                            lbl_804788B8 = 0;
                        }
                    }
                    fn_80106D3C(1, 0x3b52, 1, 1);
                    {
                        f32 acc;
                        acc = lbl_8047BA30;
                        while (acc < lbl_8047BA34) {
                            _threadSwitch();
                            acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                        }
                    }
                }
                fn_801069FC(1);
            } else {
                /* === Path B: cold boot, bit9 NOT set === */
                if (fn_800A0F58() == 1) {
                    s8 anim_result_b;  /* block-local: shares the demoted r28 color */
                    fn_80106D3C(1, 0x3b50, 1, 1);
                    tmp2 = 0;
                    fn_801026A4(0x11, (u32)fn_801046B8(), (u32)&tmp2, 0, 0, 0);
                    fn_80102868(0x11, 0x2d, 0xbe);

                    /* NOTE: path B reuses the two anim vars with swapped
                     * roles vs path A (anim_result = track, anim_result_b =
                     * result) — required for the target r30/r28 mapping. */
                    {
                        f32 acc;
                        acc = lbl_8047BA30;
                        anim_result = -1;
                        while (acc < lbl_8047BA48) {
                            if (fn_801045A8(0x11, 0) == 0) {
                                anim_result_b = fn_801043A4(0x11);
                                break;
                            }
                            obj = (s8*)fn_80104704(0x11);
                            if (anim_result != obj[0x95]) {
                                acc = lbl_8047BA30;
                                anim_result = obj[0x95];
                            }
                            _threadSwitch();
                            acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                        }
                        if (acc >= lbl_8047BA48) {
                            obj = (s8*)fn_80104704(0x11);
                            anim_result_b = obj[0x95];
                        }
                    }
                    fn_80102568(0x11, 0, 1);

                    if (anim_result_b == 0) {
                        {
                            s32 is_w;
                            if ((OSGetResetCode() + 0x80000000u) == 0u)
                                is_w = 1;
                            else
                                is_w = 0;
                            if (is_w == 1) {
                                if (fn_800A0F58() == 0) {
                                    lbl_804788B8 = 1;
                                }
                            } else {
                                lbl_804788B8 = 1;
                            }
                        }
                        fn_80106D3C(1, 0x3b51, 1, 1);
                        {
                            f32 acc;
                            acc = lbl_8047BA30;
                            while (acc < lbl_8047BA34) {
                                _threadSwitch();
                                acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                            }
                        }
                    } else {
                        {
                            s32 is_w;
                            if ((OSGetResetCode() + 0x80000000u) == 0u)
                                is_w = 1;
                            else
                                is_w = 0;
                            if (is_w == 1) {
                                if (fn_800A0F58() == 1) {
                                    lbl_804788B8 = 0;
                                }
                            } else {
                                lbl_804788B8 = 0;
                            }
                        }
                        fn_80106D3C(1, 0x3b52, 1, 1);
                        {
                            f32 acc;
                            acc = lbl_8047BA30;
                            while (acc < lbl_8047BA34) {
                                _threadSwitch();
                                acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                            }
                        }
                    }
                    fn_801069FC(1);
                } else {
                    /* === Path C: no_input, card fail === */
                    fn_801C40F0(1);
                    {
                        f32 acc;
                        acc = lbl_8047BA30;
                        while (acc < lbl_8047BA34) {
                            _threadSwitch();
                            acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                        }
                    }
                }
            }
        } else {
            /* === Path D: cold boot, fn_800AA498 != 1 === */
            fn_801C40F0(1);
            {
                f32 acc;
                acc = lbl_8047BA30;
                while (acc < lbl_8047BA34) {
                    _threadSwitch();
                    acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
            }
            fn_800A0FC8(0);
        }
    } else {
        /* === Path E: warm boot === */
        fn_801C40F0(1);
        {
            f32 acc;
            acc = lbl_8047BA30;
            while (acc < lbl_8047BA34) {
                _threadSwitch();
                acc += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
        }
    }

    /* === Shared tail: wait for fn_8017B2CC(0xa) >= 0 === */
    {
        const u8* str = lbl_80267050;
        s32 res;
        for (;;) {
            res = fn_8017B2CC(0xa);
            if (res >= 0) break;
            fn_800DD970((const char*)str);
            if (res != 0) _threadSwitch();
        }
    }

    fn_80166E88(2, 1, 0x20, 1, 0x10);
    fn_80165F84();

    /* Create GS thread */
    {
        u8* stack;
        u32* arg;
        u32 ctr_new;
        u32 ctr_old = lbl_8047A460;
        stack = ctx + 0x1640;
        *(u32*)(ctx + 0x1318) = 1;
        ctr_new = ctr_old + 1;
        arg = (u32*)(ctx + 0x1318);
        lbl_8047A460 = ctr_new;
        arg[1] = 0;
        arg[2] = 0;
        arg[3] = 0;
        fn_800A19CC(ctx + 0x1328, (void*)fn_800370E0, arg, stack + 0xffc, 0x1000, 0x10, 1);
        fn_800A1F94(ctx + 0x1328);
    }

    /* Wait for thread completion */
    while ((s32)lbl_8047A460 != 0) {
        _threadSwitch();
    }
}

/* 0x8003708C | size: 0x54 */
extern void fn_8017AC40(u32 a, u32 b, u32 c, u32 d);
extern void fn_800FE834(u32 a, u32 b, u32 c, void* fn);
extern void fn_8017AAA4(void);
s32 fn_8003708C(void) {
    fn_8017AC40(0x40, 0, 0, 0);
    fn_800FE834(1, 0x14, 0, fn_8017AAA4);
    lbl_8047A464 = 1;
    return 0;
}

/* 0x800370E0 | size: 0x78 */
s32 fn_800370E0(u32* arg) {
    if (arg[0]) fn_801655D4(arg[0]);
    if (arg[1]) fn_801655D4(arg[1]);
    if (arg[2]) fn_801655D4(arg[2]);
    if (arg[3]) fn_801655D4(arg[3]);
    lbl_8047A460--;
    return 0;
}


/* ===== Phase 2 recovery stubs ===== */

/* fn_80037158 - 0x80037158 | size: 0x4 */
#if 0
asm void fn_80037158(void) {
#include "src/game/scene_init_fn_80037158.inc"
}
#else
#pragma optimization_level 4
void fn_80037158(void) {
}
#endif

/* fn_8003715C - 0x8003715C | size: 0x18 */
extern u8 lbl_8047A470;
#if 0
asm void fn_8003715C(void) {
#include "src/game/scene_init_fn_8003715C.inc"
}
#else
#pragma optimization_level 4
u32 fn_8003715C(void) {
    u8 old = lbl_8047A470;
    lbl_8047A470 = 1;
    return 1 - old;
}
#endif

/* fn_80037174 - 0x80037174 | size: 0xc */
extern u8 lbl_8047A470;
#if 0
asm void fn_80037174(void) {
#include "src/game/scene_init_fn_80037174.inc"
}
#else
#pragma optimization_level 4
void fn_80037174(void) {
    lbl_8047A470 = 0;
}
#endif

/* fn_80037180 - 0x80037180 | size: 0x170 */
extern u32 fn_80005748(void);
extern u32 fn_801EF214(void);
extern u32 fn_801EF274(void);
extern void fn_80132A38(u32 a, u32 b);
extern u32 fn_80109934(u8* a);
extern void fn_800D88DC(u32 a);
extern void fn_800D888C(u32 a);
extern void fn_800D6A00(u32 a);
extern void fn_800D7820(void* a);
extern void fn_800D85D4(u32 a, u32 b);
extern void fn_800D67BC(u32 a);
extern void fn_800D61E4(s32 a, s32 b);
extern void fn_800D5CB8(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800D59B8(u32 a, f32 b, f32 c);
extern void fn_800D6728(void);
extern u32 lbl_8047A498;
extern u32 lbl_8047A49C;
extern u32 lbl_8047A4A0;
extern u8 lbl_80314F98[];
extern f32 lbl_8047BA58;
extern f32 lbl_8047BA5C;
/* BSS lbl_803A6498 state machine.
 * r30=p (param r4), r31=local var. 96.7% match - beq/b vs bne peephole (3 insns). */
#if 0
asm void fn_80037180(void) {
#include "src/game/scene_init_fn_80037180.inc"
}
#else
#pragma peephole off
void fn_80037180(u32 unused, u8* p) {
    u32 result;
    if (*(s16*)(p + 6) == 0x36c) {
        result = fn_80005748();
        if (lbl_8047A498 != 0) {
            result = lbl_8047A498;
        }
        lbl_8047A49C = fn_801EF214();
        lbl_8047A4A0 = fn_801EF274();
        fn_80132A38(0x4c, result);
        fn_80132A38(0x2f, fn_801EF274());
        fn_80132A38(0x30, fn_801EF214());
        if ((result = fn_80109934(lbl_803A6498)) != 0) {
            fn_800D88DC(3);
            fn_800D888C(4);
            fn_800D6A00(7);
            fn_800D7820(lbl_80314F98);
            fn_800D85D4(0, result);
            fn_800D67BC(2);
            fn_800D61E4(0, 0);
            fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
            fn_800D59B8(0, lbl_8047BA58, lbl_8047BA58);
            fn_800D61E4(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
            fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
            fn_800D59B8(0, lbl_8047BA5C, lbl_8047BA5C);
            fn_800D6728();
        }
    } else {
        if ((s32)lbl_8047A49C == 0) {
            *(s8*)(p + 4) = (s8)(p[4] & ~2);
        }
    }
}
#pragma peephole on
#endif

/* fn_800372F0 - 0x800372F0 | size: 0x4 */
#if 0
asm void fn_800372F0(void) {
#include "src/game/scene_init_fn_800372F0.inc"
}
#else
#pragma optimization_level 4
void fn_800372F0(void) {
}
#endif

/* fn_800372F4 - 0x800372F4 | size: 0x4 */
#if 0
asm void fn_800372F4(void) {
#include "src/game/scene_init_fn_800372F4.inc"
}
#else
#pragma optimization_level 4
void fn_800372F4(void) {
}
#endif

/* fn_800372F8 - 0x800372F8 | size: 0x4 */
#if 0
asm void fn_800372F8(void) {
#include "src/game/scene_init_fn_800372F8.inc"
}
#else
#pragma optimization_level 4
void fn_800372F8(void) {
}
#endif

/* fn_800372FC - 0x800372FC | size: 0x4 */
#if 0
asm void fn_800372FC(void) {
#include "src/game/scene_init_fn_800372FC.inc"
}
#else
#pragma optimization_level 4
void fn_800372FC(void) {
}
#endif

/* fn_80037300 - 0x80037300 | size: 0xc8 */
extern void fn_800D5BA0(u32 a, u32 b);
extern u8 lbl_80314E08[];
#if 0
asm void fn_80037300(void) {
#include "src/game/scene_init_fn_80037300.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
void fn_80037300(u32 unused, u8* p) {
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820(lbl_80314E08);
    fn_800D6A00(6);
    fn_800D67BC(4);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, 0x003B6DFF);
    fn_800D61E4(*(s16*)(p + 0x54), 0);
    fn_800D5BA0(0, 0x489DECFF);
    fn_800D61E4(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
    fn_800D5BA0(0, 0x489DECFF);
    fn_800D61E4(0, *(s16*)(p + 0x56));
    fn_800D5BA0(0, 0x003B6DFF);
    fn_800D6728();
}
#pragma pop
#endif

/* fn_800373C8 - 0x800373C8 | size: 0xa0 */
#if 0
asm void fn_800373C8(void) {
#include "src/game/scene_init_fn_800373C8.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
void fn_800373C8(u8* ctx, u8* p) {
    s32 v;
    v = (s32)*(f32*)(lbl_803A654C + 0x58);
    if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
    p[0x67] = (u8)(v + 0xff);
    *(s16*)(p + 0x50) = (s16)(*(f32*)(lbl_803A64EC + 0x54) + *(f32*)(lbl_803A654C + 0x58));
    if ((s8)ctx[0x95] == 3) {
        p[0x64] = 0xff; p[0x65] = 0xff; p[0x66] = 0xff;
    } else {
        p[0x64] = 0x46; p[0x65] = 0x8f; p[0x66] = 0xb4;
    }
}
#pragma peephole on
#endif

/* fn_80037468 - 0x80037468 | size: 0xa0 */
#if 0
asm void fn_80037468(void) {
#include "src/game/scene_init_fn_80037468.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
void fn_80037468(u8* ctx, u8* p) {
    s32 v;
    v = (s32)*(f32*)(lbl_803A654C + 0x40);
    if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
    p[0x67] = (u8)(v + 0xff);
    *(s16*)(p + 0x50) = (s16)(*(f32*)(lbl_803A64EC + 0x3c) + *(f32*)(lbl_803A654C + 0x40));
    if ((s8)ctx[0x95] == 2) {
        p[0x64] = 0xff; p[0x65] = 0xff; p[0x66] = 0xff;
    } else {
        p[0x64] = 0x46; p[0x65] = 0x8f; p[0x66] = 0xb4;
    }
}
#pragma peephole on
#endif

/* fn_80037508 - 0x80037508 | size: 0xa0 */
#if 0
asm void fn_80037508(void) {
#include "src/game/scene_init_fn_80037508.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
void fn_80037508(u8* ctx, u8* p) {
    s32 v;
    v = (s32)*(f32*)(lbl_803A654C + 0x28);
    if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
    p[0x67] = (u8)(v + 0xff);
    *(s16*)(p + 0x50) = (s16)(*(f32*)(lbl_803A64EC + 0x24) + *(f32*)(lbl_803A654C + 0x28));
    if ((s8)ctx[0x95] == 1) {
        p[0x64] = 0xff; p[0x65] = 0xff; p[0x66] = 0xff;
    } else {
        p[0x64] = 0x46; p[0x65] = 0x8f; p[0x66] = 0xb4;
    }
}
#pragma peephole on
#endif

/* fn_800375A8 - 0x800375A8 | size: 0xa0 */
#if 0
asm void fn_800375A8(void) {
#include "src/game/scene_init_fn_800375A8.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
void fn_800375A8(u8* ctx, u8* p) {
    s32 v;
    v = (s32)*(f32*)(lbl_803A654C + 0x10);
    if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
    p[0x67] = (u8)(v + 0xff);
    *(s16*)(p + 0x50) = (s16)(*(f32*)(lbl_803A64EC + 0x0c) + *(f32*)(lbl_803A654C + 0x10));
    if ((s8)ctx[0x95] == 0) {
        p[0x64] = 0xff; p[0x65] = 0xff; p[0x66] = 0xff;
    } else {
        p[0x64] = 0x46; p[0x65] = 0x8f; p[0x66] = 0xb4;
    }
}
#pragma peephole on
#endif

/* fn_80037648 - 0x80037648 | size: 0x80 */
extern void fn_800FB680(s32 a, u32 b, u32 c, u32 d);
extern s8 lbl_8047A490;
#if 0
asm void fn_80037648(void) {
#include "src/game/scene_init_fn_80037648.inc"
}
#else
#pragma optimization_level 4
void fn_80037648(void) {
    u32 r6;
    switch (lbl_8047A490) {
    case 0: r6 = 0x1b5b; break;
    case 1: r6 = 0x1b5c; break;
    case 2: r6 = 0x1b5d; break;
    case 3: r6 = 0x1b5e; break;
    default: r6 = 0x1b59; break;
    }
    fn_800FB680(-4, 0, (u32)-1, r6);
}
#endif

/* fn_800376C8 - 0x800376C8 | size: 0x30 */
extern u32 lbl_8047A480;
#if 0
asm void fn_800376C8(void) {
#include "src/game/scene_init_fn_800376C8.inc"
}
#else
#pragma optimization_level 4
void fn_800376C8(void) {
    fn_800FB680(0, 0, (u32)-1, lbl_8047A480);
}
#endif

/* fn_800376F8 - 0x800376F8 | size: 0xbc */
extern f32 lbl_8047BA58;
extern f32 lbl_8047A494;
#if 0
asm void fn_800376F8(void) {
#include "src/game/scene_init_fn_800376F8.inc"
}
#else
#pragma optimization_level 4
#pragma fp_contract on
void fn_800376F8(u32 unused, u8* p) {
    f32 f1, f3, f4;
    s32 v;
    f1 = *(f32*)(lbl_803A654C + 0x58);
    if (f1 < lbl_8047BA58) {
        v = (s32)f1;
        if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
        p[0x67] = (u8)(v + 0xff);
        f4 = *(f32*)(lbl_803A654C + 0x50);
        f3 = *(f32*)(lbl_803A654C + 0x58) + f4;
        *(f32*)(lbl_803A654C + 0x58) = f3;
        *(f32*)(lbl_803A654C + 0x50) = lbl_8047A494 * *(f32*)(lbl_803A654C + 0x4c) + f4;
        if (f3 > lbl_8047BA58) {
            *(f32*)(lbl_803A654C + 0x58) = lbl_8047BA58;
            *(s16*)(p + 0x50) = (s16)*(f32*)(lbl_803A654C + 0x54);
        }
        *(s16*)(p + 0x50) = (s16)(*(f32*)(lbl_803A654C + 0x54) + *(f32*)(lbl_803A654C + 0x58));
    }
}
#pragma fp_contract off
#endif

/* fn_800377B4 - 0x800377B4 | size: 0xbc */
extern f32 lbl_8047BA58;
extern f32 lbl_8047A494;
#if 0
asm void fn_800377B4(void) {
#include "src/game/scene_init_fn_800377B4.inc"
}
#else
#pragma optimization_level 4
#pragma fp_contract on
void fn_800377B4(u32 unused, u8* p) {
    f32 f1, f3, f4;
    s32 v;
    f1 = *(f32*)(lbl_803A654C + 0x40);
    if (f1 < lbl_8047BA58) {
        v = (s32)f1;
        if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
        p[0x67] = (u8)(v + 0xff);
        f4 = *(f32*)(lbl_803A654C + 0x38);
        f3 = *(f32*)(lbl_803A654C + 0x40) + f4;
        *(f32*)(lbl_803A654C + 0x40) = f3;
        *(f32*)(lbl_803A654C + 0x38) = lbl_8047A494 * *(f32*)(lbl_803A654C + 0x34) + f4;
        if (f3 > lbl_8047BA58) {
            *(f32*)(lbl_803A654C + 0x40) = lbl_8047BA58;
            *(s16*)(p + 0x50) = (s16)*(f32*)(lbl_803A654C + 0x3c);
        }
        *(s16*)(p + 0x50) = (s16)(*(f32*)(lbl_803A654C + 0x3c) + *(f32*)(lbl_803A654C + 0x40));
    }
}
#pragma fp_contract off
#endif

/* fn_80037870 - 0x80037870 | size: 0xbc */
extern f32 lbl_8047BA58;
extern f32 lbl_8047A494;
#if 0
asm void fn_80037870(void) {
#include "src/game/scene_init_fn_80037870.inc"
}
#else
#pragma optimization_level 4
void fn_80037870(u32 unused, u8* p) {
    f32 f1, f3, f4;
    s32 v;
    f1 = *(f32*)(lbl_803A654C + 0x28);
    if (f1 < lbl_8047BA58) {
        v = (s32)f1;
        if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
        p[0x67] = (u8)(v + 0xff);
        f4 = *(f32*)(lbl_803A654C + 0x20);
        f3 = *(f32*)(lbl_803A654C + 0x28) + f4;
        *(f32*)(lbl_803A654C + 0x28) = f3;
        *(f32*)(lbl_803A654C + 0x20) = lbl_8047A494 * *(f32*)(lbl_803A654C + 0x1c) + f4;
        if (f3 > lbl_8047BA58) {
            *(f32*)(lbl_803A654C + 0x28) = lbl_8047BA58;
            *(s16*)(p + 0x50) = (s16)(s32)*(f32*)(lbl_803A654C + 0x24);
        }
        *(s16*)(p + 0x50) = (s16)(s32)(*(f32*)(lbl_803A654C + 0x24) + *(f32*)(lbl_803A654C + 0x28));
    }
}
#endif

/* fn_8003792C - 0x8003792C | size: 0xbc */
extern f32 lbl_8047BA58;
extern f32 lbl_8047A494;
#if 0
asm void fn_8003792C(void) {
#include "src/game/scene_init_fn_8003792C.inc"
}
#else
#pragma optimization_level 4
void fn_8003792C(u32 unused, u8* p) {
    f32 f1, f3, f4;
    s32 v;
    f1 = *(f32*)(lbl_803A654C + 0x10);
    if (f1 < lbl_8047BA58) {
        v = (s32)f1;
        if ((s16)v < -(s16)0xff) { v = -(s32)0xff; }
        p[0x67] = (u8)(v + 0xff);
        f4 = *(f32*)(lbl_803A654C + 0x8);
        f3 = *(f32*)(lbl_803A654C + 0x10) + f4;
        *(f32*)(lbl_803A654C + 0x10) = f3;
        *(f32*)(lbl_803A654C + 0x8) = lbl_8047A494 * *(f32*)(lbl_803A654C + 0x4) + f4;
        if (f3 > lbl_8047BA58) {
            *(f32*)(lbl_803A654C + 0x10) = lbl_8047BA58;
            *(s16*)(p + 0x50) = (s16)(s32)*(f32*)(lbl_803A654C + 0xc);
        }
        *(s16*)(p + 0x50) = (s16)(s32)(*(f32*)(lbl_803A654C + 0xc) + *(f32*)(lbl_803A654C + 0x10));
    }
}
#endif

/* fn_800379E8 - 0x800379E8 | size: 0x1c8 */
extern u8* fn_80105624(void);
extern f64 fn_800CE148(f32 a);
extern f64 fn_800CDBE0(f32 a);
extern u8 lbl_802E52B8[];
extern f32 lbl_8047A484;
extern f32 lbl_8047BA58;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA64;
extern f32 lbl_8047A494;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern u8 lbl_8047A47D;
extern u8 lbl_8047A47C;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA70;
#if 1
asm void fn_800379E8(void) {
#include "src/game/scene_init_fn_800379E8.inc"
}
#else
void fn_800379E8(void) { /* TODO - 58.3% best */ }
#endif

/* fn_80037BB0 - 0x80037BB0 | size: 0x1c8 */
extern f32 lbl_8047A484;
extern f32 lbl_8047BA58;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA64;
extern f32 lbl_8047A494;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern u8 lbl_8047A47D;
extern u8 lbl_8047A47C;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA70;
#if 1
asm void fn_80037BB0(void) {
#include "src/game/scene_init_fn_80037BB0.inc"
}
#else
void fn_80037BB0(void) { /* TODO */ }
#endif

/* fn_80037D78 - 0x80037D78 | size: 0x1c8 */
extern f32 lbl_8047A484;
extern f32 lbl_8047BA58;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA64;
extern f32 lbl_8047A494;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern u8 lbl_8047A47D;
extern u8 lbl_8047A47C;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA70;
#if 1
asm void fn_80037D78(void) {
#include "src/game/scene_init_fn_80037D78.inc"
}
#else
void fn_80037D78(void) { /* TODO */ }
#endif

/* fn_80037F40 - 0x80037F40 | size: 0x1e4 */
extern u8 lbl_8047A47C;
extern f32 lbl_8047A484;
extern u8 lbl_802E52A8[];
extern f32 lbl_8047BA58;
extern f32 lbl_8047A488;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA64;
extern f32 lbl_8047A494;
extern f32 lbl_8047A48C;
extern u8 lbl_8047A47D;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA70;
#if 1
asm void fn_80037F40(void) {
#include "src/game/scene_init_fn_80037F40.inc"
}
#else
void fn_80037F40(void) { /* TODO */ }
#endif

/* fn_80038124 - 0x80038124 | size: 0x14 */
extern f32 lbl_8047BA74;
extern f32 lbl_8047A478;
#if 0
asm void fn_80038124(void) {
#include "src/game/scene_init_fn_80038124.inc"
}
#else
#pragma optimization_level 4
void fn_80038124(u32 unused, u8* p) {
    *(f32*)(p + 0x70) = lbl_8047BA74 - lbl_8047A478;
}
#endif

/* fn_80038138 - 0x80038138 | size: 0x38 */
extern f32 lbl_8047BA78;
extern f32 lbl_8047A494;
extern f32 lbl_8047A478;
extern f32 lbl_8047BA60;
#if 0
asm void fn_80038138(void) {
#include "src/game/scene_init_fn_80038138.inc"
}
#else
#pragma optimization_level 4
void fn_80038138(u32 unused, u8* p) {
    f32 f1 = lbl_8047BA78 * lbl_8047A494 + (lbl_8047A478);
    lbl_8047A478 = f1;
    if (f1 > lbl_8047BA60) {
        lbl_8047A478 = f1 - lbl_8047BA60;
    }
    *(f32*)(p + 0x70) = lbl_8047A478;
}
#endif

/* fn_80038170 - 0x80038170 | size: 0xe0 */
extern f32 lbl_8047BA58;
extern u8 lbl_802E5288[];
extern u8 lbl_8047A47C;
#if 0
asm void fn_80038170(void) {
#include "src/game/scene_init_fn_80038170.inc"
}
#else
#pragma optimization_level 4
void fn_80038170(u8* ctx, u8* p) {
    u32 flag;
    u32 off;
    flag = (*(f32*)(lbl_803A654C + 0x10) == lbl_8047BA58 &&
            *(f32*)(lbl_803A654C + 0x28) == lbl_8047BA58 &&
            *(f32*)(lbl_803A654C + 0x40) == lbl_8047BA58 &&
            *(f32*)(lbl_803A654C + 0x58) == lbl_8047BA58);
    if (flag) {
        *(s8*)(p + 4) |= 2;
    } else {
        *(s8*)(p + 4) &= (s8)~2;
    }
    lbl_8047A47C = ctx[0x95];
    off = (u32)(s32)(s8)ctx[0x95] * 8;
    *(s16*)(p + 0x50) = (s16)(s32)*(f32*)(lbl_802E5288 + off);
    *(s16*)(p + 0x52) = (s16)(s32)*(f32*)(lbl_802E5288 + off + 4);
}
#endif

/* fn_80038250 - 0x80038250 | size: 0x98 */
extern void fn_801080CC(u32 a, u32 b);
#if 0
asm void fn_80038250(void) {
#include "src/game/scene_init_fn_80038250.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80038250(u8* p) {
    s8 v = *(s8*)(p + 1);
    switch (v) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x1b, 0xc4);
            *(p + 2) = 1;
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x1b, 0xc8);
            *(p + 2) = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
#endif

/* fn_800382E8 - 0x800382E8 | size: 0x98 */
#if 0
asm void fn_800382E8(void) {
#include "src/game/scene_init_fn_800382E8.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_800382E8(u8* p) {
    s8 v = *(s8*)(p + 1);
    switch (v) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x1a, 0xbc);
            *(p + 2) = 1;
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x1a, 0xc0);
            *(p + 2) = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80038380 - 0x80038380 | size: 0xac | WALL 89.88% (W2: instruction scheduling — stb moved earlier, cmpwi moved later) */
extern s32 fn_801020C0(u32 a);
#if 0
asm void fn_80038380(void) {
#include "src/game/scene_init_fn_80038380.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
u32 fn_80038380(u8* p) {
    u8* ctx;
    u16 flags;
    s32 count;
    s8 cur;

    ctx = fn_80105624();
    flags = *(u16*)(ctx + 6);
    if (flags & 2) {
        count = (s8)fn_801020C0(*(u32*)(p + 4));
        cur = *(s8*)(p + 0x95) + 1;
        *(s8*)(p + 0x95) = cur;
        if (cur >= count) {
            *(s8*)(p + 0x95) = count - 1;
        }
    }
    flags = *(u16*)(ctx + 6);
    if (flags & 1) {
        cur = *(s8*)(p + 0x95) - 1;
        *(s8*)(p + 0x95) = cur;
        if (cur < 0) {
            *(p + 0x95) = 0;
        }
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8003842C - 0x8003842C | size: 0x498 */
extern void fn_801661D0(void);
extern void fn_8010A5BC(void);
extern void fn_8010A010(void);
extern void fn_8018F6F4(void);
extern void fn_80038A0C(void);
extern void fn_8003A520(void);
extern void fn_80058150(void);
extern u32 fn_80055E38(s32 idx);
extern void fn_80058804(u8* a, s32 b);
extern void fn_800587D8(void);
extern void fn_800599AC(u32 param);
extern void fn_80056A80(void);
extern void fn_80057A38(void);
extern void fn_80056B74(u32 a, s32 b);
extern void fn_80057A64(u8* state, u32 b);
extern void fn_80057948(void);
extern void fn_80056854(void);
extern u32 fn_800576B4(void);
extern u32 fn_80057538(void);
extern void fn_800574FC(u8* src);
extern void fn_800576C4(u32 state);
extern u8* fn_800574E0(void);
extern void fn_800574A8(void);
extern void fn_80057400(void);
extern u32 fn_800573C0(void);
extern u32 fn_800566E8(void);
extern u32 fn_80057694(void);
extern void fn_800576A4(u32 a);
extern void fn_800567AC(void);
extern void fn_80056704(void);
extern u32 fn_80057F94(u32 bgmId);
extern u32 fn_80057DE8(u32 a);
extern s32 fn_80057E40(void);
extern u32 fn_80057A08(void);
extern u32 fn_80058F08(u32* out, s32 idx);
extern void fn_80057094(s16* a, s16* b);
extern void fn_8004E9C0(s32 a);
extern s32 fn_8004DC18(u32 a);
extern u8 fn_8004DFCC(u8 a);
extern void fn_800473E0(u8* entry);
extern u8 fn_8004BDEC(void);
extern u8 fn_8004BDFC(void);
extern void fn_800492CC(u8* a, u8* b);
extern u32 fn_80043728(u32 unused, s32 mode, u16 flags);
extern void fn_800484A4(void);
extern void fn_80042658(u32 a, u32 b);
extern void fn_800439BC(void* a);
extern void fn_80041E48(u32 a, u32 b);
extern void fn_80047CC0(void);
extern void fn_800478B4(void);
extern void fn_8003F040(void);
extern void fn_8003F2DC(u8* arr, s32 count, s32 dir);
extern void fn_8003D8CC(void);
extern void fn_8003DC54(void);
extern void fn_8003E394(void);
extern void fn_8003CF38(void);
extern void fn_8003C2B8(void);
extern u32 fn_8003CE1C(u32 a);
extern s32 fn_8003ACE8(u32 a, u32 b, u32 c);
extern u32 fn_8003AE84(void);
extern void fn_8018F4C8(void);
extern void menuModelSetMotion(void);
extern void fn_8010A420(u8* a);
extern void fn_801660D8(void);
extern void fn_800FF660(void);
extern void* memcpy(void* dst, const void* src, u32 n);
extern u8 lbl_802E51C8[];
extern u8 lbl_802EF0A8[];
extern u32 lbl_8047A480;
extern u8 lbl_8047A47C;
extern f32 lbl_8047A488;
extern f32 lbl_8047A484;
extern u8 lbl_8047A47D;
extern s8 lbl_8047A490;
#if 1
asm void fn_8003842C(void) {
#include "src/game/scene_init_fn_8003842C.inc"
}
#else
void fn_8003842C(void) { /* TODO */ }
#endif

/* fn_800388C4 - 0x800388C4 | size: 0xcc */
#if 0
asm void fn_800388C4(void) {
#include "src/game/scene_init_fn_800388C4.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling off
void fn_800388C4(void) {
    fn_80102568(0x19, 0, 1);
    fn_80102568(0x1a, 0, 1);
    fn_80102568(0x1b, 0, 1);
    fn_80102568(0x18, 0, 1);
    fn_80102568(0x1e, 0, 1);
    fn_80102568(0x1f, 0, 1);
    fn_80102568(0x20, 0, 1);
    fn_80102568(0x21, 0, 1);
    fn_80102568(0x22, 0, 1);
    fn_80102568(0x23, 0, 1);
    fn_80102568(0x1d, 0, 1);
}
#pragma scheduling on
#endif

/* fn_80038990 - 0x80038990 | size: 0x70 */
extern f64 lbl_8047BA80;
extern f64 lbl_8047BA88;
extern f32 lbl_8047A494;
/* Infinite loop: compute ratio of progress/tick as f32.
 * 85.7% match - f29/f30 register swap for bias constants. */
#if 0
asm void fn_80038990(void) {
#include "src/game/scene_init_fn_80038990.inc"
}
#else
#pragma optimization_level 4
void fn_80038990(void) {
    f32 tick_f;
    for (;;) {
        tick_f = (f32)(s32)fn_800D37CC();
        lbl_8047A494 = (f32)(u32)fn_800D3088() / tick_f;
        _threadSwitch();
    }
}
#endif

/* fn_80038A00 - 0x80038A00 | size: 0xc */
extern f32 lbl_8047BA58;
extern f32 lbl_8047A484;
#if 0
asm void fn_80038A00(void) {
#include "src/game/scene_init_fn_80038A00.inc"
}
#else
#pragma optimization_level 4
void fn_80038A00(void) {
    lbl_8047A484 = lbl_8047BA58;
}
#endif

/* fn_80038A0C - 0x80038A0C | size: 0x468 */
extern void fn_8001E074(void);
extern void fn_801D036C(void);
extern void fn_80129280(void);
extern void fn_801D0748(void);
extern void fn_80135168(void);
extern void* fn_8012A5B0();
extern void fn_800056EC(void);
extern void fn_801D0314(void);
extern void fn_8012F11C(void);
extern void fn_8012A450(void);
extern void fn_80135CD0(void);
extern void fn_80135C28(void);
extern void fn_80135BF8(void);
extern void fn_80135BE0(void);
extern void fn_8011418C(void);
extern void fn_80135B6C(void);
extern void fn_80135B4C(void);
extern void fn_80135B3C(void);
extern u32 lbl_8047A480;
extern u32 lbl_8047A498;
extern u32 lbl_8047A49C;
extern u32 lbl_8047A4A0;
extern f64 lbl_8047BA88;
extern u8 lbl_8047A47C;
extern f32 lbl_8047A488;
extern f32 lbl_8047A484;
extern u8 lbl_8047A47D;
#if 1
asm void fn_80038A0C(void) {
#include "src/game/scene_init_fn_80038A0C.inc"
}
#else
void fn_80038A0C(void) { /* TODO */ }
#endif

/* fn_80038E74 - 0x80038E74 | size: 0x190 */
extern u8 lbl_80267060[];
/* Search lbl_80267060 (8 x 0x18-byte entries) for matching scene ID.
 * CW unrolls the search into 8 compare-and-branch blocks. */
#if 1
asm void fn_80038E74(void) {
#include "src/game/scene_init_fn_80038E74.inc"
}
#else
void fn_80038E74(void) { /* TODO */ }
#endif

/* fn_80039004 - 0x80039004 | size: 0x78 | WALL 89.17% (W2+W1: extsb. vs extsb+cmpwi codegen idiom, lfsx indexed vs lfs offset load, mulli r6 vs r0 regalloc) */
#if 0
asm void fn_80039004(void) {
#include "src/game/scene_init_fn_80039004.inc"
}
#else
#pragma optimization_level 4
s32 fn_80039004(u8* ctx, u8* p) {
    s32 idx;
    u32 off;
    idx = (s8)ctx[0x95];
    if (idx < 0 || idx >= 8) { idx = 0; }
    off = (u32)idx * 0xc;
    *(s16*)(p + 0x50) = (s16)(s32)*(f32*)(lbl_803A65B0 + off);
    *(s16*)(p + 0x52) = (s16)(s32)*(f32*)(lbl_803A65B0 + off + 4);
    p[0x67] = (u8)(s32)*(f32*)(lbl_803A65B0 + off + 8);
    return 0;
}
#endif

/* fn_8003907C - 0x8003907C | size: 0xac | WALL 89.88% (W2: instruction scheduling — same stb/cmpwi reorder as fn_80038380) */
#if 0
asm void fn_8003907C(void) {
#include "src/game/scene_init_fn_8003907C.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
u32 fn_8003907C(u8* p) {
    u8* ctx;
    u16 flags;
    s32 count;
    s8 cur;

    ctx = fn_80105624();
    flags = *(u16*)(ctx + 6);
    if (flags & 2) {
        count = (s8)fn_801020C0(*(u32*)(p + 4));
        cur = *(s8*)(p + 0x95) + 1;
        *(s8*)(p + 0x95) = cur;
        if (cur >= count) {
            *(s8*)(p + 0x95) = count - 1;
        }
    }
    flags = *(u16*)(ctx + 6);
    if (flags & 1) {
        cur = *(s8*)(p + 0x95) - 1;
        *(s8*)(p + 0x95) = cur;
        if (cur < 0) {
            *(p + 0x95) = 0;
        }
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_80039128 - 0x80039128 | size: 0x370 */
extern u32 lbl_8047BAA8;
extern u32 lbl_8047BAA0;
extern u32 lbl_8047BA90;
extern u32 lbl_8047BA94;
extern u32 lbl_8047BA98;
#if 1
asm void fn_80039128(void) {
#include "src/game/scene_init_fn_80039128.inc"
}
#else
void fn_80039128(void) { /* TODO */ }
#endif

/* fn_80039498 - 0x80039498 | size: 0xb0 */
extern void menuCloseSync(u32 a, u32 b);
extern u8 lbl_80267120[];
#if 0
asm u32 fn_80039498(u32 a) {
#include "src/game/scene_init_fn_80039498.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
u32 fn_80039498(u32 a) {
    u32 buf[5];
    s32 idx;
    buf[0] = a;
    buf[1] = ((u32*)lbl_80267120)[0];
    buf[2] = ((u32*)lbl_80267120)[1];
    buf[3] = ((u32*)lbl_80267120)[2];
    buf[4] = ((u32*)lbl_80267120)[3];
    idx = fn_801026A4(0x24, (u32)fn_801046B8(), (u32)buf, 0, 1, 0);
    fn_80102510(0x24);
    menuCloseSync(0x24, 1);
    if (idx < 0 || idx >= 4) { return 4; }
    return buf[1 + idx];
}
#endif

/* fn_80039548 - 0x80039548 | size: 0x24 */
extern u32 lbl_8047A4B0;
#if 0
asm void fn_80039548(void) {
#include "src/game/scene_init_fn_80039548.inc"
}
#else
#pragma optimization_level 4
u32 fn_80039548(u32 unused, u8* p) {
    *(u32*)(p + 0x4c) = (s32)lbl_8047A4B0 == 0 ? 0x1B6D : 0x1B6E;
    return 0;
}
#endif

/* fn_8003956C - 0x8003956C | size: 0x98 */
extern u32 fn_801347D0(u32 a);
extern void* fn_80134768(void* base, s16 index);
extern u8 fn_801429E8(void* a);
extern void fn_80109220(u32 a, u32 b);
extern u32 lbl_8047A4A8;
/* Count alive entries; if count+1 > threshold, enable.
 * r27=p, r28=threshold, r29=total, r30=i, r31=count. 71.1% - regalloc. */
#if 0
asm void fn_8003956C(u32 unused, u8* p) {
#include "src/game/scene_init_fn_8003956C.inc"
}
#else
#pragma push
#pragma peephole off
u32 fn_8003956C(u32 unused, u8* p) {
    s32 count;
    s32 i;
    u16 total;
    u32 threshold;

    count = 0;
    threshold = lbl_8047A4A8 + 8;
    total = (u16)fn_801347D0(0);
    for (i = 0; i < (s32)total; i++) {
        if ((u8)fn_801429E8(fn_80134768(0, (s16)i)) != 0) {
            count++;
        }
    }
    {
        s32 r4;
        if ((s32)threshold < count + 1) { r4 = 1; } else { r4 = 0; }
        fn_80109220((u32)p, (u32)r4);
    }
    return 0;
}
#pragma pop
#endif

/* fn_80039604 - 0x80039604 | size: 0x40 */
extern u32 lbl_8047A4A8;
#if 0
asm void fn_80039604(void) {
#include "src/game/scene_init_fn_80039604.inc"
}
#else
#pragma optimization_level 4
u32 fn_80039604(u32 unused, u32 a) {
    fn_80109220(a, (s32)lbl_8047A4A8 > 0 ? 1 : 0);
    return 0;
}
#endif

/* fn_80039644 - 0x80039644 | size: 0x290 */
extern u32 fn_800FA444(u32 a);
extern u16 fn_80143C68(void* a);
extern u32 fn_801440A0(u32 idx);
extern void fn_80144088(void);
extern void fn_80144014(void);
extern void fn_80143C50(void);
extern u32 lbl_8047A4A8;
extern f32 lbl_8047BAB0;
extern f32 lbl_8047A4C0;
extern u32 lbl_8047A4BC;
#if 1
asm void fn_80039644(u8* a, u32 b) {
#include "src/game/scene_init_fn_80039644.inc"
}
#else
void fn_80039644(u8* a, u32 b) { /* TODO */ }
#endif

/* fn_800398D4 - 0x800398D4 | size: 0x58 */
extern void fn_800FE38C(s32 a, s32 b, s32 c, s32 d);
extern void fn_800FE35C(void);
#if 0
asm void fn_800398D4(void) {
#include "src/game/scene_init_fn_800398D4.inc"
}
#else
#pragma optimization_level 4
u32 fn_800398D4(u8* p, u8* q) {
    fn_800FE38C(0x118 - *(s16*)(q + 0x50), 0x8b - *(s16*)(q + 0x52), 0x150, 0x10d);
    fn_80039644(p + 0x94, *(u32*)(p + 0x88));
    fn_800FE35C();
    return 0;
}
#endif

/* fn_8003992C - 0x8003992C | size: 0x44 */
extern u32 lbl_8047A4AC;
extern u32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
#if 0
asm void fn_8003992C(void) {
#include "src/game/scene_init_fn_8003992C.inc"
}
#else
#pragma optimization_level 4
u32 fn_8003992C(u32 unused, u8* p) {
    s32 r = (s32)(lbl_8047A4AC * 31 + 0x9a);
    if ((s32)lbl_8047A4BC == 0) {
        r += (s32)lbl_8047A4C0;
    }
    *(s16*)(p + 0x52) = (s16)r;
    return 0;
}
#endif

/* fn_80039970 - 0x80039970 | size: 0xe0 */
extern u32 lbl_8047A4B8;
extern u32 lbl_8047A4A8;
extern u32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern u32 lbl_8047A4AC;
#if 0
asm void fn_80039970(void) {
#include "src/game/scene_init_fn_80039970.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 fn_80039970(u32 unused, u8* p) {
    s32 v;
    s32 sum;
    u32 flag;
    if ((s32)lbl_8047A4B8 >= 0) {
        v = (s32)(lbl_8047A4B8 - lbl_8047A4A8) * 0x1f + 0x97;
        if ((s32)lbl_8047A4BC != 0) {
            v -= (s32)lbl_8047A4C0;
        }
        sum = v + *(s16*)(p + 0x56);
        if (sum < 0x97 || v >= 0x18f) {
            flag = 0;
        } else {
            flag = 1;
        }
        fn_80109220((u32)p, flag);
    } else {
        v = (s32)lbl_8047A4AC * 0x1f + 0x97;
        if ((s32)lbl_8047A4BC == 0) {
            v += (s32)lbl_8047A4C0;
        }
        fn_80109220((u32)p, 1);
    }
    *(s16*)(p + 0x52) = (s16)v;
    return 0;
}
#endif

/* fn_80039F44 - 0x80039F44 | size: 0x2c */
extern void fn_80102ED4(u8* a);
extern u32 lbl_8047A4B8;
#if 0
asm void fn_80039F44(void) {
#include "src/game/scene_init_fn_80039F44.inc"
}
#else
#pragma optimization_level 4
void fn_80039F44(u8* a) {
    if ((s32)lbl_8047A4B8 < 0) {
        fn_80102ED4(a);
    }
}
#endif

/* fn_80039F70 - 0x80039F70 | size: 0x19c */
extern u32 fn_80143F84(void* a);
extern u32 lbl_8047A4A8;
extern u32 lbl_8047A4AC;
extern u32 lbl_8047A4B4;
extern f32 lbl_8047A4C0;
extern u32 lbl_8047A4BC;
extern u32 lbl_8047A4B8;
extern f32 lbl_8047BABC;
#if 0
asm void fn_80039F70(void) {
#include "src/game/scene_init_fn_80039F70.inc"
}
#else
#pragma optimization_level 4
void fn_80039F70(u8* ctx) {
    s8 state;
    void* r28;
    u32 r30;
    u32 r29;
    u16 r26;
    u32 r27;
    u32 r3;
    f32 f0;
    f32 f1;
    f32 f2;
    state = (s8)ctx[1];
    if (state == 2) { goto lbl_case2; }
    if (state > 2) {
        if (state >= 4) { return; }
        goto lbl_case3;
    }
    if (state != 0) { return; }
    /* state == 0 */
    if ((s8)ctx[2] != 0) { return; }
    fn_801080CC(0x25, 0xb4);
    r27 = lbl_8047A4A8 + lbl_8047A4AC;
    r26 = (u16)fn_801347D0(0);
    r30 = (u32)-1;
    r29 = 0;
    r28 = NULL;
    while (r29 < (u32)r26) {
        r28 = fn_80134768(NULL, (s16)r29);
        if ((u8)fn_801429E8(r28) != 0) {
            r30++;
            if (r30 >= r27) { goto lbl_found; }
        }
        r29++;
    }
    r3 = 0;
    goto lbl_store;
lbl_found:
    r3 = (u32)(u16)fn_80143C68(r28);
lbl_store:
    if (r3 != 0) {
        r3 = fn_80143F84((void*)fn_801440A0(r3));
    } else {
        r3 = 0x1b68;
    }
    lbl_8047A4B4 = r3;
    lbl_8047A4C0 = lbl_8047BAB0;
    lbl_8047A4BC = 0;
    lbl_8047A4B8 = (u32)-1;
    ctx[2] = 1;
    return;
lbl_case2:
    f2 = lbl_8047A4C0;
    f1 = lbl_8047BAB0;
    if (f2 > f1) {
        f0 = f2 - lbl_8047BABC;
        lbl_8047A4C0 = f0;
        if (f0 < f1) {
            lbl_8047A4C0 = f1;
        }
    }
    f2 = lbl_8047A4C0;
    f1 = lbl_8047BAB0;
    if (f2 >= f1) { return; }
    f0 = f2 + lbl_8047BABC;
    lbl_8047A4C0 = f0;
    if (f0 <= f1) { return; }
    lbl_8047A4C0 = f1;
    return;
lbl_case3:
    if ((s8)ctx[2] != 0) { return; }
    fn_801080CC(0x25, 0xb8);
    ctx[2] = 1;
}
#endif

/* fn_8003A10C - 0x8003A10C | size: 0x414 */
extern void fn_801298B8(void);
extern void fn_801069FC(void);
extern void fn_80129A78(void);
extern void fn_80134584(void);
extern u32 lbl_8047A4B0;
extern u32 lbl_8047A4A8;
extern u32 lbl_8047A4AC;
extern u32 lbl_8047A4B4;
#if 1
asm void fn_8003A10C(u32 a) {
#include "src/game/scene_init_fn_8003A10C.inc"
}
#else
void fn_8003A10C(u32 a) { /* TODO */ }
#endif

/* fn_8003A520 - 0x8003A520 | size: 0x1a0 */
extern void fn_8017B3E4(u32 a);
extern s32  fn_8017B2CC(u32 a);
extern void fn_80018F54(u32 a, u32 b, u32 c);
extern void fn_8017B1CC(u32 a);
extern void fn_800F915C(u32 a);
#if 0
asm void fn_8003A520(void) {
#include "src/game/scene_init_fn_8003A520.inc"
}
#else
#pragma optimization_level 4
void fn_8003A520(void) {
    s32 r = 0;
    for (;;) {
        r = fn_80039498(r);
        if ((u32)(r - 3) <= 1) { break; }
        switch (r) {
        case 0:
            fn_80102510(0x19);
            fn_80102510(0x1b);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1b, 1);
            fn_8003A10C(0);
            fn_8010264C(0x1b, 0);
            fn_8010264C(0x19, 0);
            break;
        case 1:
            fn_80102510(0x19);
            fn_80102510(0x1a);
            fn_80102510(0x1b);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1a, 1);
            menuCloseSync(0x1b, 1);
            fn_8017B3E4(0x66f);
            while (fn_8017B2CC(0x66f) == 1) { _threadSwitch(); }
            fn_80018F54(4, 0, 0);
            fn_8017B1CC(0x66f);
            fn_800F915C(0x66f);
            fn_8010264C(0x1a, 0);
            fn_8010264C(0x1b, 0);
            fn_8010264C(0x19, 0);
            break;
        case 2:
            fn_80102510(0x19);
            fn_80102510(0x1b);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1b, 1);
            fn_8003A10C(1);
            fn_8010264C(0x1b, 0);
            fn_8010264C(0x19, 0);
            break;
        }
    }
}
#endif

/* fn_8003A6C0 - 0x8003A6C0 | size: 0x130 */
extern void fn_800FB8C8(s32 a, s32 b, s32 c, s32 d, u32 e, u32 f);
extern u8 lbl_80267130[];
extern u32 lbl_8047A4C8;
#if 0
asm void fn_8003A6C0(void) {
#include "src/game/scene_init_fn_8003A6C0.inc"
}
#else
#pragma optimization_level 4
u32 fn_8003A6C0(u8* ctx, u8* p) {
    s32 idx;
    s32 pw;
    s32 digit;
    s32 val;
    s32* tbl = (s32*)lbl_80267130;
    s32 t0, t1, t2;

    idx = 0;
    t0 = tbl[0]; t1 = tbl[1]; t2 = tbl[2];
    val = *(s16*)(p + 6);
    if (val != t0) {
        idx = 1;
        if (val != t1) {
            idx = 2;
            if (val != t2) {
                idx = 3;
            }
        }
    }

    if (idx >= 3) { return 0; }

    /* compute 10^idx */
    {
        s32 i;
        pw = 1;
        for (i = 0; i < idx; i++) {
            pw *= 10;
        }
    }

    digit = (s32)lbl_8047A4C8 / pw % 10;
    fn_80132A38(0x34, digit);
    fn_800FB8C8(0, 0, *(s16*)(p + 0x54), *(s16*)(p + 0x56), *(u32*)(ctx + 0x88), 0xc9);
    return 0;
}
#endif

/* fn_8003A7F0 - 0x8003A7F0 | size: 0x460 */
extern void fn_80166A50(u32 a, u32 b, u32 c, u32 d);
extern u32 lbl_8047A4C8;
#if 1
asm void fn_8003A7F0(void) {
#include "src/game/scene_init_fn_8003A7F0.inc"
}
#else
void fn_8003A7F0(void) { /* TODO */ }
#endif

/* fn_8003AC50 - 0x8003AC50 | size: 0x98 */
#if 0
asm void fn_8003AC50(void) {
#include "src/game/scene_init_fn_8003AC50.inc"
}
#else
#pragma optimization_level 4
u32 fn_8003AC50(u8* p) {
    s8 v = *(s8*)(p + 1);
    switch (v) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x26, 0xb4);
            *(p + 2) = 1;
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x26, 0xb8);
            *(p + 2) = 1;
        }
        break;
    }
    return 0;
}
#endif

/* fn_8003ACE8 - 0x8003ACE8 | size: 0x84 */
extern u32 lbl_8047A4C8;
#if 0
asm void fn_8003ACE8(void) {
#include "src/game/scene_init_fn_8003ACE8.inc"
}
#else
#pragma optimization_level 4
s32 fn_8003ACE8(u32 a, u32 b, u32 c) {
    u32 buf[2];
    s32 r;
    lbl_8047A4C8 = a;
    buf[0] = c;
    buf[1] = b;
    r = fn_801026A4(0x26, (u32)fn_801046B8(), 0, 0, 1, 1, (u32)buf);
    fn_80102510(0x26);
    menuCloseSync(0x26, 1);
    if (r == -1) { return -1; }
    return (s32)lbl_8047A4C8;
}
#endif

/* fn_8003AE84 - 0x8003AE84 | size: 0x6c */
#if 0
asm void fn_8003AE84(void) {
#include "src/game/scene_init_fn_8003AE84.inc"
}
#else
#pragma optimization_level 0
u32 fn_8003AE84(void) {
    s32 result;

    result = fn_8010264C(0x27, 1);
    fn_80102510(0x27);
    menuCloseSync(0x27, 1);
    if (result == -1) {
        return 0;
    }
    if (result == 0) {
        return 1;
    }
    return 0;
}
#endif

/* fn_8003AFDC - 0x8003AFDC | size: 0x2fc */
extern u8 fn_801EE8F4(u32 a);
extern u8 fn_80109B90(u8* a, u32 b);
extern void fn_801EED88(u32 a);
extern u32 fn_801EE614(u32 a);
extern u8 fn_801EEAD0(u32 a);
extern u8 fn_801EEC74(u32 a);
extern void fn_801040F0(s32 a, s32 b, void* c, s32 d, s32 e);
extern u8 lbl_803A6748[];
extern u32 lbl_8047A4D4;
extern u32 lbl_8047BAC4;
extern u32 lbl_8047BAC8;
#if 0
asm void fn_8003AFDC(void) {
#include "src/game/scene_init_fn_8003AFDC.inc"
}
#else
#pragma optimization_level 4
void fn_8003AFDC(u8* ctx, u8* p) {
    u32 result;
    u32 full_val;
    u32 r26;
    u32 r31;
    u8* tbl;
    u8 eead0_r;
    u8 eec74_r;
    s32 case_val;
    u32 r6;
    u8 eead0_r2;
    u8 eec74_r2;
    switch ((s32)*(s16*)(p + 6)) {
    case 0xd96:
    case 0xd97:
    case 0xd99:
    case 0x308:
    case 0x30b:
        break;
    case 0xd98:
        if ((u8)fn_801EE8F4((u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + *(u32*)lbl_803A6748 * 4 + 2)) != 0) {
            u8* base_4c = lbl_803A6748 + 0x4c;
            if ((u8)fn_80109B90(base_4c, 0) == 0) {
                if ((result = fn_80109934(base_4c)) != 0) {
                    fn_800D88DC(3);
                    fn_800D888C(4);
                    fn_800D6A00(7);
                    fn_800D7820(lbl_80314F98);
                    fn_800D85D4(0, result);
                    fn_800D67BC(2);
                    fn_800D61E4(0, 0);
                    fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
                    fn_800D59B8(0, *(f32*)&lbl_8047BAC4, *(f32*)&lbl_8047BAC4);
                    fn_800D61E4(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
                    fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
                    fn_800D59B8(0, *(f32*)&lbl_8047BAC8, *(f32*)&lbl_8047BAC8);
                    fn_800D6728();
                }
            }
        }
        break;
    default:
        full_val = *(u32*)lbl_803A6748;
        r31 = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + 2 + full_val * 4);
        fn_801EED88((u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + 2 + (full_val & 0xFFFF) * 4));
        tbl = (u8*)lbl_802EF0A8;
        r26 = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + (*(u32*)lbl_803A6748 & 0xFFFF) * 4 + 2);
        fn_801EE614(r26);
        fn_801EE8F4(r26);
        eead0_r = fn_801EEAD0(r26);
        eec74_r = fn_801EEC74(r26);
        if ((u8)eead0_r != 0) {
            case_val = ((u8)eec74_r != 0) ? 1 : 0;
        } else {
            case_val = 2;
        }
        switch (case_val) {
        case 0:
            fn_801040F0(
                (s32)(s16)(*(s16*)(tbl + 0x55fa) - *(s16*)(p + 0x50) - 7),
                (s32)(s16)(*(s16*)(tbl + 0x55fc) - *(s16*)(p + 0x52) - 4),
                ctx, 0x161, 0);
            break;
        case 1:
            fn_801040F0(
                (s32)(s16)(*(s16*)(tbl + 0x55fa) - *(s16*)(p + 0x50)),
                (s32)(s16)(*(s16*)(tbl + 0x55fc) - *(s16*)(p + 0x52)),
                ctx, 0x160, 0);
            break;
        case 2:
        default:
            break;
        }
        eead0_r2 = (u8)fn_801EE8F4(r31);
        eec74_r2 = (u8)fn_801EEC74(r31);
        if (eead0_r2 == 0) {
            r6 = 0x36e8;
        } else if (eec74_r2 == 0) {
            r6 = 0x36e6;
        } else {
            r6 = 0x36e7;
        }
        fn_800FB680(0, 0, (s32)((u32)ctx[0x8b] | 0xFFFFFF00u), r6);
        break;
    }
}
#endif

/* fn_8003B2D8 - 0x8003B2D8 | size: 0x1a0 */
extern u32 fn_801EE544(u32 a, void* b);
extern u32 fn_801EEFAC(u32 a, u32 b);
extern u32 fn_801EE328(u32 a);
extern u32 fn_801FCCC4();
extern u32 fn_801FCC7C(void);
extern u32 fn_800FA280();
extern f32 lbl_8047BAC0;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D4;
#if 0
asm void fn_8003B2D8(u8* ctx) {
#include "src/game/scene_init_fn_8003B2D8.inc"
}
#else
#pragma optimization_level 4
void fn_8003B2D8(u8* ctx) {
    u8* base;
    u32 r27, r28, r29, r30;
    u32 r0;
    u32 val;
    s32 field94;
    base = (u8*)lbl_803A6748;
    ctx[0x8b] = (u8)(s32)(lbl_8047BAC0 * *(f32*)(base + 0x44));
    r30 = 0;
    r0 = *(u32*)base & 0xFFFF;
    if (lbl_8047A4D0 == 0) {
        r29 = r30;
    } else {
        r27 = (u32)*(u16*)((u8*)lbl_8047A4D4 + r0 * 4 + 2);
        r28 = fn_801EE544(r27, base + 0x94);
        field94 = (s32)*(u8*)(base + 0x94);
        if (field94 == 0) {
            r28 = 0x371f;
        } else if (field94 >= 3) {
            /* keep r28 */
        } else {
            if (r27 == 0x43) {
                r28 = 0x12b0;
            } else {
                r0 = fn_801EEFAC(r27, 0) & 0xFFFF;
                if (r0 == 9) {
                    r29 = fn_801EE328(r27);
                    r30 = 1;
                    goto L_B3B8;
                }
                fn_801FCCC4();
                r28 = fn_801FCC7C();
            }
        }
        L_B3B8:
        if (r30 == 0) {
            r29 = fn_800FA280(r28);
        }
    }
    r27 = r29;
    if (r29 == 0) {
        r27 = fn_800FA280(1);
    }
    val = fn_8003CE1C(*(u32*)lbl_803A6748);
    if (val == 0) {
        val = fn_800FA280(1);
    }
    fn_80132A38(0x37, val);
    fn_800FB680(0, 0, (s32)((u32)ctx[0x8b] | 0xFFFFFF00u), 0xe7);
    fn_80132A38(0x37, r27);
    fn_800FB680(0xb4, 0, (s32)((u32)ctx[0x8b] | 0xFFFFFF00u), 0xe7);
}
#endif

/* fn_8003B478 - 0x8003B478 | size: 0x258 */
extern u32 gamedataGetStatus(u32 a, u32 b);
extern void fn_801240C4(u32 a, u32 b, u32 c, u32 d);
extern u32 fn_801EE750(u32 a);
extern void fn_8011DFE0(u32 a, u32 b);
extern void fn_8011D8F4(u32 a, u32 b);
extern void fn_8011D8D8(u32 a, u32 b);
extern u32 fn_801231A4(u32 a);
extern f32 lbl_8047BAC0;
extern u32 lbl_8047A4D4;
extern u32 lbl_8047A4D0;
#if 0
asm void fn_8003B478(u8* ctx) {
#include "src/game/scene_init_fn_8003B478.inc"
}
#else
#pragma optimization_level 4
void fn_8003B478(u8* ctx) {
    u8* base;
    u32 full_val;
    u32 lo16;
    u32 r28;
    u32 r29_u16;
    u32 r26;
    u32 r27;
    u32 r30;
    u8 eead0_r;
    u8 eec74_r;
    u32 r6;
    u32 r29_u32;
    base = (u8*)lbl_803A6748;
    ctx[0x8b] = (u8)(s32)(lbl_8047BAC0 * *(f32*)(base + 0x44));
    full_val = *(u32*)base;
    r28 = lbl_8047A4D0;
    lo16 = full_val & 0xFFFF;
    r29_u16 = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + full_val * 4);
    r26 = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + 2 + full_val * 4);
    r27 = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + 2 + lo16 * 4);
    fn_801EE614(r27);
    fn_801EE8F4(r27);
    eead0_r = fn_801EEAD0(r27);
    eec74_r = fn_801EEC74(r27);
    if ((u8)eead0_r != 0) {
        r27 = ((u8)eec74_r != 0) ? 1 : 0;
    } else {
        r27 = 2;
    }
    r6 = gamedataGetStatus(0, 1);
    fn_801240C4(r28, r29_u16, 0xa, r6);
    r29_u32 = *(u32*)(base + 0x98);
    fn_801EE750(r26);
    fn_8011DFE0(r28, r29_u32);
    if (r27 == 1) {
        fn_8011D8F4(r28, 0);
        fn_8011D8D8(r28, 0);
    } else {
        fn_8011D8F4(r28, 1);
        fn_8011D8D8(r28, 0xa);
    }
    r28 = lbl_8047A4D0;
    if (r28 == 0) {
        r30 = 0x80;
    } else if ((r26 & 0xFFFF) == 0) {
        r30 = 0x80;
    } else if ((u8)fn_801EE8F4(r26) != 0) {
        fn_8011DFE0(r28, fn_801EE750(r26));
        r30 = fn_801231A4(r28);
        fn_8011DFE0(r28, *(u32*)(base + 0x98));
    } else {
        r30 = 2;
    }
    r30 = r30 & 0xFF;
    if (r30 == 0x80) {
        fn_80132A38(0x37, fn_800FA280(1));
    } else if (r30 == 0) {
        fn_80132A38(0x37, fn_800FA280(0xd67));
    } else if (r30 == 1) {
        fn_80132A38(0x37, fn_800FA280(0xd68));
    } else if (r30 == 2) {
        fn_80132A38(0x37, fn_800FA280(0xd69));
    } else {
        fn_80132A38(0x37, fn_800FA280(0xd68));
    }
    fn_800FB680(0, 0, (s32)((u32)ctx[0x8b] | 0xFFFFFF00u), 0xcf);
}
#endif

/* fn_8003B6D0 - 0x8003B6D0 | size: 0x144 */
extern u16 fn_801EE248(u32 a);
extern u32 fn_8011E778();
extern u32 fn_8011E760();
extern u32 fn_801FCC3C(u32 a);
extern u32 fn_801FCA2C(u32 a);
extern u32 fn_801FCA14(u32 a);
extern u8 fn_801FC964(u32 a);
extern f32 lbl_8047BAC0;
extern u32 lbl_8047A4D4;
#if 0
asm void fn_8003B6D0(u8* ctx) {
#include "src/game/scene_init_fn_8003B6D0.inc"
}
#else
#pragma optimization_level 4
void fn_8003B6D0(u8* ctx) {
    u8* base;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 r0;
    base = (u8*)lbl_803A6748;
    ctx[0x8b] = (u8)(lbl_8047BAC0 * *(f32*)(base + 0x44));
    r0 = *(u32*)base;
    r28 = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + r0 * 4 + 2);
    r30 = 0;
    if ((fn_801EEFAC(r28, 0) & 0xFFFF) == 9) {
        fn_801EE248(r28);
        r0 = fn_8011E778();
        if (r0 != 0) {
            r30 = fn_800FA280(fn_8011E760());
        }
    } else {
        r29 = fn_801FCA2C(fn_801FCC3C(fn_801FCCC4()));
        r30 = fn_800FA280(fn_801FCA14(r29));
        while (1) {
            r0 = (u32)(u8)fn_801FC964(r29);
            if (r28 == r0) {
                r0 = fn_801FCA14(r29);
                if (r0 != 0) {
                    r30 = fn_800FA280(r0);
                }
                break;
            }
            r29 += 0x50;
        }
    }
    if (r30 == 0) { r30 = fn_800FA280(1); }
    fn_80132A38(0x37, r30);
    fn_800FB680(0, 0, (s32)ctx[0x8b] | -0x100, 0xcf);
}
#endif

/* fn_8003B814 - 0x8003B814 | size: 0x24 */
extern void fn_8003B85C(u32 a, u32 b);
#if 0
asm void fn_8003B814(void) {
#include "src/game/scene_init_fn_8003B814.inc"
}
#else
#pragma optimization_level 4
void fn_8003B814(u32 a) {
    fn_8003B85C(a, 1);
}
#endif

/* fn_8003B838 - 0x8003B838 | size: 0x24 */
#if 0
asm void fn_8003B838(void) {
#include "src/game/scene_init_fn_8003B838.inc"
}
#else
#pragma optimization_level 4
void fn_8003B838(u32 a) {
    fn_8003B85C(a, 0);
}
#endif

/* fn_8003BF54 - 0x8003BF54 | size: 0xe8 */
extern f32 lbl_8047BAC0;
extern f64 lbl_8047BAE0;
extern f32 lbl_8047BAD8;
extern u8 lbl_802EF0A8[];
#if 0
asm void fn_8003BF54(u8* ctx, u8* p) {
#include "src/game/scene_init_fn_8003BF54.inc"
}
#else
#pragma optimization_level 4
void fn_8003BF54(u8* ctx, u8* p) {
    s32 field10;
    f32 fval;
    ctx[0x8b] = (u8)(s32)(lbl_8047BAC0 * *(f32*)(lbl_803A6748 + 0x44));
    field10 = *(s32*)(lbl_803A6748 + 0x10);
    if (field10 > 0xa) {
        p[4] = (s8)((u8)p[4] | 0x2);
        fval = (f32)(s32)(field10 - 0xa) * lbl_8047BAD8;
        *(s16*)(p + 0x52) = (s16)(s32)(*(f32*)(lbl_803A6748 + 0x18) / (-fval) * (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5088) + (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5084));
    } else {
        p[4] = (s8)((u8)p[4] & ~(u8)0x2);
    }
}
#endif

/* fn_8003C03C - 0x8003C03C | size: 0x100 */
extern f32 lbl_8047BAC0;
extern f32 lbl_8047BAF4;
extern f32 lbl_8047BAF0;
extern f64 lbl_8047BAE0;
#if 0
asm void fn_8003C03C(void) {
#include "src/game/scene_init_fn_8003C03C.inc"
}
#else
#pragma optimization_level 4
void fn_8003C03C(u8* ctx, u8* p) {
    u8* base;
    s32 field_c;
    base = (u8*)lbl_803A6748;
    ctx[0x8b] = (u8)(s32)(lbl_8047BAC0 * *(f32*)(base + 0x44));
    field_c = *(s32*)(base + 0xc);
    if (field_c >= 0xa) {
        if (*(u32*)(base + 0x10) != (u32)field_c) {
            f32 f0, f1, f3;
            *(s8*)(p + 4) = (s8)((u8)p[4] | 2);
            f0 = lbl_8047BAF4 * *(f32*)(base + 0x28);
            f0 = lbl_8047BAF4 * f0;
            f1 = lbl_8047BAF0 * f0;
            f1 = (f32)fn_800CE148(f1);
            f3 = f1;
            *(s16*)(p + 0x52) = (s16)(s32)(lbl_8047BAF4 * f3 + (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x504c));
        } else {
            *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
        }
    } else {
        *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
    }
}
#endif

/* fn_8003C13C - 0x8003C13C | size: 0xe0 */
extern f32 lbl_8047BAC0;
extern f32 lbl_8047BAF4;
extern f32 lbl_8047BAF0;
extern f64 lbl_8047BAE0;
#if 0
asm void fn_8003C13C(void) {
#include "src/game/scene_init_fn_8003C13C.inc"
}
#else
#pragma optimization_level 4
void fn_8003C13C(u8* ctx, u8* p) {
    u8* base;
    base = (u8*)lbl_803A6748;
    ctx[0x8b] = (u8)(s32)(lbl_8047BAC0 * *(f32*)(base + 0x44));
    if (*(s32*)(base + 0x8) > 0) {
        f32 f0, f1, f3;
        *(s8*)(p + 4) = (s8)((u8)p[4] | 2);
        f0 = lbl_8047BAF4 * *(f32*)(base + 0x28);
        f0 = lbl_8047BAF4 * f0;
        f1 = lbl_8047BAF0 * f0 + lbl_8047BAF0;
        f1 = (f32)fn_800CE148(f1);
        f3 = f1;
        *(s16*)(p + 0x52) = (s16)(s32)(lbl_8047BAF4 * f3 + (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5014));
    } else {
        *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
    }
}
#endif

/* fn_8003C21C - 0x8003C21C | size: 0x30 */
extern f32 lbl_8047BAC0;
#if 0
asm void fn_8003C21C(void) {
#include "src/game/scene_init_fn_8003C21C.inc"
}
#else
#pragma optimization_level 4
void fn_8003C21C(u8* p) {
    p[0x8b] = (u8)(lbl_8047BAC0 * *(f32*)(lbl_803A6748 + 0x44));
}
#endif

/* fn_8003C24C - 0x8003C24C | size: 0x6c */
extern f32 lbl_8047BAC0;
#if 0
asm void fn_8003C24C(void) {
#include "src/game/scene_init_fn_8003C24C.inc"
}
#else
#pragma optimization_level 4
void fn_8003C24C(u8* ctx, u8* p) {
    s16 field6 = *(s16*)(p + 6);
    if (field6 == 0x2e3) {
        goto call_fallback;
    }
    if (field6 < 0x2e3) {
        if (field6 >= 0x2e2) {
            goto set_value;
        }
        goto call_fallback;
    }
    if (field6 >= 0x2e5) {
        goto call_fallback;
    }
set_value:
    {
        ctx[0x8b] = (u8)(lbl_8047BAC0 * *(f32*)(lbl_803A6748 + 0x44));
        return;
    }
call_fallback:
    fn_8003C2B8();
}
#endif

/* fn_8003C2B8 - 0x8003C2B8 | size: 0x470 */
extern f32 lbl_8047BAC0;
extern u32 lbl_8047BAF8;
extern u32 lbl_8047BAFC;
extern u32 lbl_8047BAC8;
extern f32 lbl_8047BAD8;
extern u32 lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047BB00;
extern u32 lbl_8047A4DC;
#if 1
asm void fn_8003C2B8(void) {
#include "src/game/scene_init_fn_8003C2B8.inc"
}
#else
void fn_8003C2B8(void) { /* TODO */ }
#endif

/* fn_8003C728 - 0x8003C728 | size: 0x98 */
extern f32 lbl_8047BAC0;
extern f64 lbl_8047BAE0;
#if 0
asm void fn_8003C728(void) {
#include "src/game/scene_init_fn_8003C728.inc"
}
#else
#pragma optimization_level 4
void fn_8003C728(u8* ctx, u8* p) {
    s32 diff;
    s32 off;
    ctx[0x8b] = (u8)(s32)(lbl_8047BAC0 * *(f32*)(lbl_803A6748 + 0x44));
    diff = (s32)(*(u32*)(lbl_803A6748 + 0x0) - *(u32*)(lbl_803A6748 + 0x8));
    if (diff >= 0xa) {
        diff = 9;
    }
    off = *(s16*)(lbl_802EF0A8 + 0x50a0) + diff * 0x18;
    *(f32*)(lbl_803A6748 + 0x38) = (f32)(s32)off;
    *(s16*)(p + 0x52) = (s16)(s32)*(f32*)(lbl_803A6748 + 0x30);
}
#endif

/* fn_8003C7C0 - 0x8003C7C0 | size: 0x65c */
extern void fn_80124410(void);
extern void fn_80166AB8(u32 a, u32 b, u32 c);
extern void fn_80109C88(void);
extern void fn_801EEDEC(void);
extern u32 fn_800E202C(u32 a);
extern void fn_800E24B0(u32 a);
extern void fn_800E209C(u32 a);
extern u32 lbl_8047BAC4;
extern u32 lbl_8047BAC8;
extern u8 lbl_803A6610[];
extern u32 lbl_8047A4DC;
extern f64 lbl_8047BAE0;
extern u32 lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047BAD4;
#if 1
asm void fn_8003C7C0(void) {
#include "src/game/scene_init_fn_8003C7C0.inc"
}
#else
void fn_8003C7C0(void) { /* TODO */ }
#endif

/* fn_8003CE1C - 0x8003CE1C | size: 0x11c */
extern u8 fn_801EE174(u32 a);
extern u32 fn_801FBD58(u32 a);
extern u32 fn_801EEF40(u32 a);
extern u32 fn_801FCC64(void);
extern u32 fn_801FBD28(void);
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D4;
#if 0
asm u32 fn_8003CE1C(u32 a) {
#include "src/game/scene_init_fn_8003CE1C.inc"
}
#else
#pragma peephole off
u32 fn_8003CE1C(u32 a) {
    u32 r31;
    u8* r30;
    u32 r29;
    u32 result;

    if (lbl_8047A4D0 == 0) { return 0; }

    r31 = (u32)*(u16*)((u8*)lbl_8047A4D4 + a * 4 + 2);
    r30 = (u8*)lbl_803A6748 + 0x94;
    fn_801EE544(r31, (void*)r30);
    {
        s32 field = (s32)*(u8*)r30;
        if (field == 2) {
            goto case2;
        }
        if (field >= 3) {
            goto done;
        }
        /* field == 0 or 1 */
        result = 0x3720;
        goto done;
    }
case2:
    {
        u32 rr;
        rr = (u32)(u16)fn_801EEFAC(r31, 0);
        if ((rr & 0xFFFF) == 9) {
            fn_801FBD58((u32)(u8)fn_801EE174(r31));
            goto check43;
        }
        r29 = (u32)(u16)fn_801EEFAC(r31, 1);
        if ((r29 & 0xFFFF) != 0) {
            if (fn_801EEF40(r31) != 3) {
                rr = r29;
            }
        }
        fn_801FCCC4(rr & 0xFFFF);
        fn_801FCC64();
        fn_801FBD58(0);
    }
check43:
    if (r31 == 0x43) {
        result = 0x12c0;
        goto done;
    }
    result = fn_801FBD28();
done:
    return fn_800FA280(result);
}
#endif

/* fn_8003CF38 - 0x8003CF38 | size: 0x2c4 */
extern u32 fn_800E2C04(u32 a, u32 b);
extern u32 fn_800E27B0(void);
extern u32 fn_800E202C(u32 a);
extern void fn_800E24B0(u32 a);
extern void fn_800E209C(u32 a);
extern u32 fn_801EE0BC(void);
extern u32 lbl_8047A4DC;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D4;
extern u32 lbl_8047A4D8;
#if 0
asm void fn_8003CF38(void) {
#include "src/game/scene_init_fn_8003CF38.inc"
}
#else
void fn_8003CF38(void) {
    u8 tmp[8];
    u8* sortbuf;
    s32 count;
    s32 i;
    s32 r29;
    s32 r31;
    s32 r28;
    s32 j;
    s32 gap;
    s32 gap8;
    s32 ii;
    s32 jj;
    u8* a;
    u8* b;
    u32 r3;
    u16 r27;
    u32 handle;

    lbl_8047A4DC = 0x60;
    lbl_8047A4D0 = 0;
    r3 = fn_800E2C04(0x140, 0x20);
    if ((u16)r3 != 0) {
        r3 = fn_800E27B0();
    } else {
        r3 = 0;
    }
    lbl_8047A4D0 = r3;

    r3 = fn_800E2C04(((lbl_8047A4DC * 4) + 0x1f) & ~0x1f, 0x20);
    if ((u16)r3 != 0) {
        r3 = fn_800E27B0();
    } else {
        r3 = 0;
    }
    lbl_8047A4D4 = r3;

    r3 = fn_800E2C04(((lbl_8047A4DC * 8) + 0x1f) & ~0x1f, 0x20);
    if ((u16)r3 != 0) {
        r3 = fn_800E27B0();
    } else {
        r3 = 0;
    }
    lbl_8047A4D8 = r3;

    r31 = 0;
    i = r31;
    r28 = 0;
    r29 = 0;
    while (r29 < (s32)lbl_8047A4DC) {
        r27 = fn_801EE248((u32)(u16)(r29 + 1));
        if ((u16)r27 != 0) {
            if ((u8)fn_801EE8F4((u32)(u16)(r29 + 1)) == 0) {
                if ((u16)fn_801EE614((u32)(u16)(r29 + 1)) == 0) {
                    goto skip;
                }
            }
            *(u16*)((u8*)lbl_8047A4D4 + r31) = r27;
            *(u16*)((u8*)lbl_8047A4D4 + r31 + 2) = (u16)(r29 + 1);
            r3 = fn_801EE0BC();
            *(u32*)((u8*)lbl_8047A4D8 + i) = r3;
            r28++;
            *(u16*)((u8*)lbl_8047A4D8 + i + 4) = *(u16*)((u8*)lbl_8047A4D4 + r31);
            *(u16*)((u8*)lbl_8047A4D8 + i + 6) = *(u16*)((u8*)lbl_8047A4D4 + r31 + 2);
            i += 8;
            r31 += 4;
        }
    skip:
        r29++;
    }

    lbl_8047A4DC = (u32)r28;
    sortbuf = (u8*)lbl_8047A4D8;
    count = r28;
    for (gap = count / 2; gap > 0; gap = gap / 2) {
        gap8 = gap * 8;
        for (j = gap; j < count; j++) {
            jj = j - gap;
            a = sortbuf + jj * 8;
            goto test;
            do {
                memcpy(tmp, a, 8);
                memcpy(a, b, 8);
                memcpy(b, tmp, 8);
                a -= gap8;
                jj -= gap;
            test:
                if (jj < 0) { break; }
                b = sortbuf + (jj + gap) * 8;
            } while ((s32)*(u32*)a > (s32)*(u32*)b);
        }
    }

    {
        s32 k = 0;
        s32 d8off = 0;
        s32 d4off = 0;
        while (k < (s32)lbl_8047A4DC) {
            *(u16*)((u8*)lbl_8047A4D4 + d4off) =
                *(u16*)((u8*)lbl_8047A4D8 + d8off + 4);
            d8off += 8;
            k++;
            d4off += 4;
            *(u16*)((u8*)lbl_8047A4D4 + d4off - 4 + 2) =
                *(u16*)((u8*)lbl_8047A4D8 + d8off - 8 + 6);
        }
    }

    handle = fn_800E202C(lbl_8047A4D8);
    if ((u16)handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
    lbl_8047A4D8 = 0;
}
#endif

/* fn_8003D4C8 - 0x8003D4C8 | size: 0x350 */
extern u16 fn_8025FE84(u32 a, u16 b);
extern u16 fn_8025FEE4(u32 a);
extern u32 fn_8025FDDC(u32 a, u32 b);
extern u32 fn_8025FD34(u32 a, u32 b);
extern void fn_8011DF90(u32 a, u32 b);
extern u32 fn_8011F5C8();
extern void fn_8011E1D4(void);
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E8;
extern u8 lbl_803A6818[];
extern u32 lbl_8047A4E0;
#if 1
asm void fn_8003D4C8(void) {
#include "src/game/scene_init_fn_8003D4C8.inc"
}
#else
void fn_8003D4C8(void) { /* TODO */ }
#endif

/* fn_8003D818 - 0x8003D818 | size: 0xb4 */
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E8;
extern u16 fn_8025FE84(u32 a, u16 b);
extern u16 fn_8025FEE4(u32 a);
#if 0
asm void fn_8003D818(void) {
#include "src/game/scene_init_fn_8003D818.inc"
}
#else
#pragma optimization_level 4
void fn_8003D818(void) {
    u8* base;
    u16 count;
    u16 i;
    base = (u8*)lbl_803A6818;
    count = 0;
    i = count;
    while ((u16)i < (u16)fn_8025FEE4(0)) {
        *(u16*)((u8*)lbl_8047A4E4 + i * 2) = fn_8025FE84(0, i);
        count++;
        i++;
    }
    *(u16*)&lbl_8047A4E8 = count;
    base[0x158] = 0;
    base[0x159] = 0;
    base[0x15b] = 0;
    base[0x15a] = 0;
    base[0x15c] = 0;
    fn_8003E394();
    *(u32*)(lbl_803A6818 + 0x10) = (u32)*(u16*)&lbl_8047A4E8;
}
#endif

/* fn_8003D8CC - 0x8003D8CC | size: 0x388 */
extern void fn_800F9D04(void);
extern void fn_800F96E4(u8* dst, u32 len, u8* src);
extern u8 lbl_802E60B0[];
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E8;
#if 1
asm void fn_8003D8CC(void) {
#include "src/game/scene_init_fn_8003D8CC.inc"
}
#else
void fn_8003D8CC(void) { /* TODO */ }
#endif

/* fn_8003DC54 - 0x8003DC54 | size: 0x740 */
extern u8 fn_8012640C(u32 a, u16 b, u16 c, u16 d);
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E8;
#if 1
asm void fn_8003DC54(void) {
#include "src/game/scene_init_fn_8003DC54.inc"
}
#else
void fn_8003DC54(void) { /* TODO */ }
#endif

/* fn_8003E394 - 0x8003E394 | size: 0xcac */
extern void fn_8011E18C(void);
extern void fn_8011E1A4(void);
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E8;
#if 1
asm void fn_8003E394(void) {
#include "src/game/scene_init_fn_8003E394.inc"
}
#else
void fn_8003E394(void) { /* TODO */ }
#endif

/* fn_8003F040 - 0x8003F040 | size: 0x29c */
extern s32 GScharCmp(u32 a, u32 b);
extern void fn_800F96E4(u8* dst, u32 len, u8* src);
extern u32 fn_800E2C04(u32 a, u32 b);
extern u32 fn_800E27B0(void);
extern u32 fn_800E202C(u32 a);
extern void fn_800E24B0(u32 a);
extern void fn_800E209C(u32 a);
extern u32 gamedataGetStatus(u32 a, u32 b);
extern void fn_801240C4(u32 a, u32 b, u32 c, u32 d);
extern u32 fn_8025FDDC(u32 a, u32 b);
extern u32 fn_8025FD34(u32 a, u32 b);
extern void fn_8011DFE0(u32 a, u32 b);
extern void fn_8011DF90(u32 a, u32 b);
/* fn_8011F5C8, fn_8011E778, fn_8011E760 declared above as K&R-style */
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E8;
#if 0
asm void fn_8003F040(void) {
#include "src/game/scene_init_fn_8003F040.inc"
}
#else
void fn_8003F040(void) {
    u8 tmp[0x1a];
    u8* base;
    u16* idx;
    s32 count;
    u32 i;
    s32 j;
    s32 gap;
    s32 k;
    u32 src_id;
    u32 lookup;
    u32 ctx;
    u32 r3;
    u32 a;
    u8* p;
    u8* q;
    u32 handle;

    r3 = fn_800E2C04(0x28a0, 0x20);
    if ((u16)r3 != 0) {
        r3 = fn_800E27B0();
    } else {
        r3 = 0;
    }
    base = (u8*)r3;

    r3 = fn_800E2C04(0x320, 0x20);
    if ((u16)r3 != 0) {
        r3 = fn_800E27B0();
    } else {
        r3 = 0;
    }
    idx = (u16*)r3;

    for (i = 0; (u16)i < (u16)(*(u16*)&lbl_8047A4E8); i++) {
        u32 ii = (u16)i;
        src_id = ((u16*)lbl_8047A4E4)[ii];
        idx[ii] = (u16)src_id;
        lookup = src_id;
        ctx = lbl_8047A4E0;
        if (ctx != 0) {
            if (src_id >= 0x8000) {
                lookup = src_id & 0x3FFF;
            }
            r3 = gamedataGetStatus(0, 1);
            fn_801240C4(ctx, lookup & 0xFFFF, 0xa, r3);
            a = fn_8025FDDC(0, lookup);
            lookup = fn_8025FD34(0, lookup);
            fn_8011DFE0(ctx, a);
            fn_8011DF90(ctx, lookup);
            r3 = lbl_8047A4E0;
        } else {
            r3 = 0;
        }
        if (r3 != 0) {
            fn_8011F5C8();
            fn_8011E778();
            r3 = fn_8011E760();
        } else {
            r3 = 0;
        }
        p = base + ii * 0x1a;
        fn_800F96E4(p + 2, 0x18, (u8*)r3);
        *(u16*)p = i;
    }

    count = (s32)*(u16*)&lbl_8047A4E8;
    for (gap = count / 2; gap > 0; gap = gap / 2) {
        for (j = gap; j < count; j++) {
            k = j - gap;
            p = base + k * 0x1a;
            goto test_lbl;
            do {
                memcpy(tmp, p, 0x1a);
                memcpy(p, q, 0x1a);
                memcpy(q, tmp, 0x1a);
                p -= gap * 0x1a;
                k -= gap;
            test_lbl:
                if (k < 0) { break; }
                q = base + (k + gap) * 0x1a;
            } while (GScharCmp((u32)(p + 2), (u32)(q + 2)) >= 0);
        }
    }

    for (i = 0; (u16)i < (u16)(*(u16*)&lbl_8047A4E8); i++) {
        u32 ii = (u16)i;
        ((u16*)lbl_8047A4E4)[ii] = idx[*(u16*)(base + ii * 0x1a)];
    }

    if (base != 0) {
        handle = fn_800E202C((u32)base);
        if ((u16)handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
    if (idx != 0) {
        handle = fn_800E202C((u32)idx);
        if ((u16)handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
}
#endif

/* fn_8003F2DC - 0x8003F2DC | size: 0x188 */
#if 0
asm void fn_8003F2DC(void) {
#include "src/game/scene_init_fn_8003F2DC.inc"
}
#else
void fn_8003F2DC(u8* arr, s32 count, s32 dir) {
    s32 gap;
    s32 i;
    s32 j;
    u8* a;
    u8* b;
    u8 tmp[4];

    if (dir == 0) {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                a = arr + (i - gap) * 4;
                j = i - gap;
                while (j >= 0) {
                    b = arr + (j + gap) * 4;
                    if (*(u16*)(a + 2) <= *(u16*)(b + 2)) { break; }
                    memcpy(tmp, a, 4);
                    memcpy(a, b, 4);
                    memcpy(b, tmp, 4);
                    a -= gap * 4;
                    j -= gap;
                }
            }
            gap = gap / 2;
        }
    } else {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                a = arr + (i - gap) * 4;
                j = i - gap;
                while (j >= 0) {
                    b = arr + (j + gap) * 4;
                    if (*(u16*)(a + 2) >= *(u16*)(b + 2)) { break; }
                    memcpy(tmp, a, 4);
                    memcpy(a, b, 4);
                    memcpy(b, tmp, 4);
                    a -= gap * 4;
                    j -= gap;
                }
            }
            gap = gap / 2;
        }
    }
}
#endif

/* fn_8003F464 - 0x8003F464 | size: 0xbb4 */
extern void fn_800FE6D0(void);
extern void fn_800FE4D4(void);
extern void jumptable_802E60C4();
extern u8 lbl_802E52C8[];
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BCA8;
extern f32 lbl_8047BCA4;
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BCAC;
#if 1
asm void fn_8003F464(void) {
#include "src/game/scene_init_fn_8003F464.inc"
}
#else
void fn_8003F464(void) { /* TODO */ }
#endif

/* fn_80040018 - 0x80040018 | size: 0x2f0 */
extern void fn_8005DA18(u32 a);
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BCC4;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BCC8;
extern u32 lbl_804788C0;
#if 0
asm void fn_80040018(void) {
#include "src/game/scene_init_fn_80040018.inc"
}
#else
#pragma optimization_level 4
void fn_80040018(u8* p) {
    u8* base;
    f32 target, cur, step;

    base = (u8*)lbl_803A6818;

    /* lerp field 0x4c toward 0x50 */
    target = *(f32*)(base + 0x50);
    cur = *(f32*)(base + 0x4c);
    if (target != cur) {
        step = *(f32*)(base + 0x3c) / lbl_8047BCB8;
        if (target > cur) {
            cur += step;
            *(f32*)(base + 0x4c) = cur;
            if (cur > lbl_8047BCBC) {
                *(f32*)(base + 0x4c) = lbl_8047BCBC;
            }
        } else {
            cur -= step;
            *(f32*)(base + 0x4c) = cur;
            if (cur < lbl_8047BC94) {
                *(f32*)(base + 0x4c) = lbl_8047BC94;
            }
        }
    }

    /* lerp field 0x54 toward 0x58 */
    target = *(f32*)(base + 0x58);
    cur = *(f32*)(base + 0x54);
    if (target != cur) {
        step = *(f32*)(base + 0x3c) / lbl_8047BCB8;
        if (target > cur) {
            cur += step;
            *(f32*)(base + 0x54) = cur;
            if (cur > lbl_8047BCBC) {
                *(f32*)(base + 0x54) = lbl_8047BCBC;
            }
        } else {
            cur -= step;
            *(f32*)(base + 0x54) = cur;
            if (cur < lbl_8047BC94) {
                *(f32*)(base + 0x54) = lbl_8047BC94;
            }
        }
    }

    /* lerp field 0x5c toward 0x60 */
    target = *(f32*)(base + 0x60);
    cur = *(f32*)(base + 0x5c);
    if (target != cur) {
        step = *(f32*)(base + 0x3c) / lbl_8047BCC0;
        if (target > cur) {
            cur += step;
            *(f32*)(base + 0x5c) = cur;
            if (cur > lbl_8047BCBC) {
                *(f32*)(base + 0x5c) = lbl_8047BCBC;
            }
        } else {
            cur -= step;
            *(f32*)(base + 0x5c) = cur;
            if (cur < lbl_8047BC94) {
                *(f32*)(base + 0x5c) = lbl_8047BC94;
            }
        }
    }

    /* lerp field 0x1dc toward 0x1e0 */
    {
        f32 f0, f1, f2, f3, f4;
        f0 = *(f32*)(base + 0x1dc);
        f1 = *(f32*)(base + 0x1e0);
        if (f0 != f1) {
            f3 = f1 - f0;
            f4 = lbl_8047BCC4 * f3 * *(f32*)(base + 0x3c);
            if (f4 > lbl_8047BC98) {
                f4 = lbl_8047BC98;
            }
            if (f4 <= lbl_8047BCC8) {
                f4 = lbl_8047BCC8;
            }
            f1 = *(f32*)(base + 0x1dc) + f4;
            f3 = *(f32*)(base + 0x1e0);
            f2 = f3 - f1;
            *(f32*)(base + 0x1dc) = f1;
            if (f4 <= lbl_8047BC94) {
                f4 = -f4;
            }
            if (f2 <= lbl_8047BC94) {
                f0 = -f2;
            } else {
                f0 = f2;
            }
            if (f0 <= f4) {
                *(f32*)(base + 0x1dc) = f3;
            } else {
                if (f2 <= lbl_8047BC94) {
                    f2 = -f2;
                }
                if (f2 < lbl_8047BCBC) {
                    *(f32*)(base + 0x1dc) = f3;
                }
            }
        }
    }

    /* input check */
    {
        u8* ctx;
        u16 flags;
        u16 r29;
        if (*(f32*)(base + 0x50) == *(f32*)(base + 0x4c) && p != 0) {
            fn_8005DA18(*(u32*)(p + 4));
            ctx = fn_80105624();
            flags = *(u16*)(ctx + 4);
            if (lbl_804788C0 != 0) {
                r29 = flags;
                if (r29 & 0x10) {
                    fn_80166A50(0x24, 0, 0, 0);
                    p[0x98] = 1;
                    *(u32*)(lbl_803A6818 + 0x18) = 0;
                }
                if (r29 & 0x40) {
                    fn_80166A50(0x24, 0, 0, 0);
                    p[0x98] = 1;
                    *(u32*)(lbl_803A6818 + 0x18) = 1;
                }
            }
            if (flags & 0x20) {
                fn_80166A50(0x25, 0, 0, 0);
                *(u32*)(lbl_803A6818 + 0x18) = (u32)-1;
                p[0x98] = 1;
                p[0x99] = 1;
            }
        }
    }
}
#endif

/* fn_80040308 - 0x80040308 | size: 0xe0c */
extern void fn_8010A210(void);
extern void fn_80109BFC(void);
extern void fn_800E3C5C(void);
extern void fn_80190E34(void);
extern void GSscene_SetMode(u32 a);
extern void GScameraGetPerspective(void);
extern void fn_800E01F4(void);
extern void GScameraSetPosition(void);
extern void GScameraSetPerspective(void);
extern void GScameraLookAt(void);
extern void GSlightSetType(void);
extern void fn_800DCC84(void);
extern void fn_800DCC60(void);
extern void fn_800DCC3C(void);
extern void fn_800DCC34(void);
extern void GScameraSetRotation(void);
extern void fn_80103484(void);
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BCC4;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BCC8;
extern u32 lbl_8047A4E8;
extern u32 lbl_804788C0;
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E4;
extern u8 lbl_802E5448[];
extern u8 lbl_802E543C[];
extern u8 lbl_80267180[];
extern u32 lbl_8047BC9C;
extern u32 lbl_8047BCCC;
extern u8 lbl_804788D4[4];
extern f64 lbl_8047BCB0;
#if 1
asm void fn_80040308(void) {
#include "src/game/scene_init_fn_80040308.inc"
}
#else
void fn_80040308(void) { /* TODO */ }
#endif

/* fn_80041114 - 0x80041114 | size: 0x48 */
extern u8 lbl_803A67FC[];
#if 0
asm void fn_80041114(void) {
#include "src/game/scene_init_fn_80041114.inc"
}
#else
#pragma optimization_level 4
void fn_80041114(u32 a) {
    GSscene_SetMode(4);
    fn_800439BC(lbl_803A67FC);
    fn_80041E48(a, 0);
}
#endif

/* fn_8004115C - 0x8004115C | size: 0x48 */
#if 0
asm void fn_8004115C(void) {
#include "src/game/scene_init_fn_8004115C.inc"
}
#else
#pragma optimization_level 4
void fn_8004115C(u32 a) {
    GSscene_SetMode(4);
    fn_800439BC(lbl_803A67FC);
    fn_80042658(a, 1);
}
#endif

/* fn_800411A4 - 0x800411A4 | size: 0x48 */
#if 0
asm void fn_800411A4(void) {
#include "src/game/scene_init_fn_800411A4.inc"
}
#else
#pragma optimization_level 4
void fn_800411A4(u32 a) {
    GSscene_SetMode(4);
    fn_800439BC(lbl_803A67FC);
    fn_80042658(a, 0);
}
#endif

/* fn_800411EC - 0x800411EC | size: 0x10 */
#if 0
asm void fn_800411EC(void) {
#include "src/game/scene_init_fn_800411EC.inc"
}
#else
#pragma optimization_level 4
u32 fn_800411EC(void) {
    return *(u32*)(lbl_803A6818 + 0x28);
}
#endif

/* fn_800411FC - 0x800411FC | size: 0x960 */
extern void fn_8011E444(void);
extern void fn_8011CB6C(void);
extern void fn_8011CB54(void);
extern void fn_800FBB34(void);
extern void fn_8011E1BC(void);
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E4;
extern u8 lbl_803A67E8[];
extern u32 lbl_8047A4F0;
#if 1
asm void fn_800411FC(void) {
#include "src/game/scene_init_fn_800411FC.inc"
}
#else
void fn_800411FC(void) { /* TODO */ }
#endif

/* fn_80041B5C - 0x80041B5C | size: 0x74 */
extern f32 lbl_8047BCA0;
#if 0
asm void fn_80041B5C(void) {
#include "src/game/scene_init_fn_80041B5C.inc"
}
#else
#pragma optimization_level 4
void fn_80041B5C(u8* ctx, u8* p) {
    s16 field6 = *(s16*)(p + 6);
    ctx[0x8b] = (u8)(s32)(lbl_8047BCA0 * *(f32*)(lbl_803A6818 + 0x4c));
    if (field6 == 0x76a || field6 == 0x759 || field6 == 0x331 || field6 == 0xfbe) {
        return;
    }
    fn_800411FC();
}
#endif

/* fn_80041BD0 - 0x80041BD0 | size: 0x278 */
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E4;
extern f32 lbl_8047BCA0;
extern u8 lbl_802E554C[];
#if 1
asm void fn_80041BD0(void) {
#include "src/game/scene_init_fn_80041BD0.inc"
}
#else
void fn_80041BD0(void) { /* TODO */ }
#endif

/* fn_80041E48 - 0x80041E48 | size: 0x810 */
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BCD8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCC4;
extern u32 lbl_8047BCD0;
extern u32 lbl_804788C4;
extern u32 lbl_8047BCD4;
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BCC8;
extern u32 lbl_8047BCAC;
#if 1
asm void fn_80041E48(u32 a, u32 b) {
#include "src/game/scene_init_fn_80041E48.inc"
}
#else
void fn_80041E48(u32 a, u32 b) { /* TODO */ }
#endif

/* fn_80042658 - 0x80042658 | size: 0x10d0 */
extern void fn_801DAC3C(void);
extern void fn_800F7994(void);
extern void fn_800F7920(void);
extern void fn_800E019C(void);
extern void fn_800E3D98(void);
extern void fn_800E43A4(void);
extern void fn_800E3BC0(void);
extern u32 lbl_804788C4;
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BCD8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCC4;
extern u32 lbl_8047BCD0;
extern u32 lbl_8047BCD4;
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BCC8;
extern f32 lbl_8047BCA4;
extern u32 lbl_8047BCE0;
extern u32 lbl_8047BCE4;
extern u32 lbl_8047BCE8;
extern u32 lbl_8047BCEC;
extern u32 lbl_8047BC9C;
extern f32 lbl_8047BCA8;
extern u32 lbl_8047BCF0;
extern u32 lbl_8047A4E4;
extern u32 lbl_804788C0;
extern u32 lbl_8047A4E0;
#if 1
asm void fn_80042658(u32 a, u32 b) {
#include "src/game/scene_init_fn_80042658.inc"
}
#else
void fn_80042658(u32 a, u32 b) { /* TODO */ }
#endif

/* fn_80043728 - 0x80043728 | size: 0x294 */
extern u32 lbl_8047A4E8;
extern f32 lbl_8047BCF4;
extern f64 lbl_8047BCB0;
extern f32 lbl_8047BCF8;
extern f32 lbl_8047BC94;
#if 0
asm void fn_80043728(void) {
#include "src/game/scene_init_fn_80043728.inc"
}
#else
#pragma optimization_level 4
u32 fn_80043728(u32 unused, s32 mode, u16 flags) {
    u16 f;
    u8* base;
    base = (u8*)lbl_803A6818;
    f = flags;
    *(u32*)(base + 0x1c) = 2;

    if (f & 2) {
        /* right arrow - advance */
        u16 total = *(u16*)&lbl_8047A4E8;
        if (total == 0) goto done;
        if (*(s32*)(base + 0x0) >= (s32)(total - 1)) goto done;
        *(u32*)(base + 0x0) = *(u32*)(base + 0x0) + 1;
        fn_80166AB8(0x23, 0, 0);
        {
            u8* b = (u8*)lbl_803A6818;
            *(f32*)(b + 0x38) = *(f32*)(b + 0x38) - lbl_8047BCF4;
            *(u32*)(b + 0x8) = *(u32*)(b + 0x8) + 1;
            *(u32*)(b + 0xc) = *(u32*)(b + 0xc) + 1;
        }
    } else if (f & 1) {
        /* left arrow - retreat */
        u16 total = *(u16*)&lbl_8047A4E8;
        if (total == 0) goto done;
        if (*(s32*)(base + 0x0) <= 0) goto done;
        *(u32*)(base + 0x0) = *(u32*)(base + 0x0) - 1;
        fn_80166AB8(0x23, 0, 0);
        {
            u8* b = (u8*)lbl_803A6818;
            *(f32*)(b + 0x38) = *(f32*)(b + 0x38) + lbl_8047BCF4;
            *(u32*)(b + 0x8) = *(u32*)(b + 0x8) - 1;
            *(u32*)(b + 0xc) = *(u32*)(b + 0xc) - 1;
        }
    } else if (f & 8) {
        /* page down */
        u16 total;
        s32 field0;
        s32 newval;
        if (mode != 0) goto done;
        total = *(u16*)&lbl_8047A4E8;
        if (total == 0) goto done;
        field0 = *(s32*)(base + 0x0);
        newval = field0 + 0xa;
        *(u32*)(base + 0x0) = (u32)newval;
        if (newval > (s32)(total - 1)) {
            s32 last = (s32)(total - 1);
            *(f32*)(base + 0x38) = -(f32)(s32)(last * 0x1b);
            *(u32*)(base + 0x0) = (u32)last;
            if (field0 != last) {
                s32 diff = last - field0;
                *(u32*)(base + 0x8) = *(u32*)(base + 0x8) + (u32)diff;
                *(u32*)(base + 0xc) = *(u32*)(base + 0xc) + (u32)diff;
            }
        } else {
            *(f32*)(base + 0x38) = *(f32*)(base + 0x38) - lbl_8047BCF8;
            *(u32*)(base + 0x8) = *(u32*)(base + 0x8) + 0xa;
            *(u32*)(base + 0xc) = *(u32*)(base + 0xc) + 0xa;
            fn_80166AB8(0x23, 0, 0);
        }
    } else if (f & 4) {
        /* page up */
        u16 total;
        s32 field0;
        s32 newval;
        if (mode != 0) goto done;
        total = *(u16*)&lbl_8047A4E8;
        if (total == 0) goto done;
        field0 = *(s32*)(base + 0x0);
        newval = field0 - 0xa;
        *(u32*)(base + 0x0) = (u32)newval;
        if (newval < 0) {
            *(u32*)(base + 0x0) = 0;
            *(f32*)(base + 0x38) = lbl_8047BC94;
            if (field0 != 0) {
                *(u32*)(base + 0x8) = (u32)-5;
                if (total >= 5) {
                    *(u32*)(base + 0xc) = 5;
                } else {
                    *(u32*)(base + 0xc) = (u32)total;
                }
                *(u32*)(lbl_803A6818 + 0x10) = (u32)total;
            }
        } else {
            *(f32*)(base + 0x38) = *(f32*)(base + 0x38) + lbl_8047BCF8;
            *(u32*)(base + 0x8) = *(u32*)(base + 0x8) - 0xa;
            *(u32*)(base + 0xc) = *(u32*)(base + 0xc) - 0xa;
            fn_80166AB8(0x23, 0, 0);
        }
    }

done:
    return 0;
}
#endif

/* fn_800439BC - 0x800439BC | size: 0x31c */
extern void fn_800F7A08(void);
extern void fn_800F7A7C(void);
extern void fn_800CE2D8(void);
extern f64 lbl_8047BCB0;
extern f32 lbl_8047BC94;
extern u32 lbl_8047BCFC;
extern u32 lbl_8047BD00;
extern u32 lbl_8047BD04;
extern u32 lbl_8047BD08;
extern u32 lbl_804788C4;
#if 1
asm void fn_800439BC(void* a) {
#include "src/game/scene_init_fn_800439BC.inc"
}
#else
void fn_800439BC(void* a) { /* TODO */ }
#endif

/* fn_80043CD8 - 0x80043CD8 | size: 0xe8 */
extern f32 lbl_8047BCA0;
extern f64 lbl_8047BCB0;
extern f32 lbl_8047BCF4;
#if 0
asm void fn_80043CD8(void) {
#include "src/game/scene_init_fn_80043CD8.inc"
}
#else
#pragma optimization_level 4
void fn_80043CD8(u8* ctx, u8* p) {
    u8* base;
    s32 field10;
    base = (u8*)lbl_803A6818;
    ctx[0x8b] = (u8)(s32)(lbl_8047BCA0 * *(f32*)(base + 0x4c));
    field10 = *(s32*)(base + 0x10);
    if (field10 > 0xa) {
        f32 f0, f3;
        s32 count_m1;
        *(s8*)(p + 4) = (s8)((u8)p[4] | 2);
        f0 = *(f32*)(base + 0x30);
        count_m1 = field10 - 1;
        f3 = -f0 / (f32)(s32)count_m1;
        *(s16*)(p + 0x52) = (s16)(s32)(f3 / lbl_8047BCF4 * (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5788) + (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5784));
    } else {
        *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
    }
}
#endif

/* fn_80043DC0 - 0x80043DC0 | size: 0x108 */
extern f32 lbl_8047BCA0;
extern u32 lbl_804788C0;
extern f32 lbl_8047BCA8;
extern f32 lbl_8047BCA4;
extern f64 lbl_8047BCB0;
extern f32 lbl_8047BD0C;
#if 0
asm void fn_80043DC0(void) {
#include "src/game/scene_init_fn_80043DC0.inc"
}
#else
#pragma optimization_level 4
void fn_80043DC0(u8* ctx, u8* p) {
    u8* base;
    s32 field10;
    s32 field0;
    base = (u8*)lbl_803A6818;
    ctx[0x8b] = (u8)(s32)(lbl_8047BCA0 * *(f32*)(base + 0x4c));
    field10 = *(s32*)(base + 0x10);
    field0 = *(s32*)(base + 0x0);
    if (field0 == field10 - 1) {
        *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
    } else if (lbl_804788C0 != 0) {
        f32 f0, f1, f3;
        *(s8*)(p + 4) = (s8)((u8)p[4] | 2);
        f0 = lbl_8047BCA8 * *(f32*)(base + 0x40);
        f0 = lbl_8047BCA8 * f0;
        f1 = lbl_8047BCA4 * f0;
        f1 = (f32)fn_800CE148(f1);
        f3 = f1;
        *(s16*)(p + 0x52) = (s16)(s32)(lbl_8047BD0C * f3 + (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x57f4));
    } else {
        *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
    }
}
#endif

/* fn_80043EC8 - 0x80043EC8 | size: 0xe0 */
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BCA8;
extern f32 lbl_8047BCA4;
extern f64 lbl_8047BCB0;
extern f32 lbl_8047BD0C;
#if 0
asm void fn_80043EC8(void) {
#include "src/game/scene_init_fn_80043EC8.inc"
}
#else
#pragma optimization_level 4
void fn_80043EC8(u8* ctx, u8* p) {
    u8* base;
    base = (u8*)lbl_803A6818;
    ctx[0x8b] = (u8)(s32)(lbl_8047BCA0 * *(f32*)(base + 0x4c));
    if (*(s32*)(base + 0x0) != 0) {
        f32 f0, f1, f3;
        *(s8*)(p + 4) = (s8)((u8)p[4] | 2);
        f0 = lbl_8047BCA8 * *(f32*)(base + 0x40);
        f0 = lbl_8047BCA8 * f0;
        f1 = lbl_8047BCA4 * f0 + lbl_8047BCA4;
        f1 = (f32)fn_800CE148(f1);
        f3 = f1;
        *(s16*)(p + 0x52) = (s16)(s32)(lbl_8047BD0C * f3 + (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x57bc));
    } else {
        *(s8*)(p + 4) = (s8)((u8)p[4] & ~(u8)2);
    }
}
#endif

/* fn_80043FA8 - 0x80043FA8 | size: 0x3d0 */
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BCA8;
extern f32 lbl_8047BCA4;
extern u32 lbl_8047BD14;
extern u32 lbl_8047BD10;
extern f32 lbl_8047BC94;
#if 1
asm void fn_80043FA8(void) {
#include "src/game/scene_init_fn_80043FA8.inc"
}
#else
void fn_80043FA8(void) { /* TODO */ }
#endif

/* fn_80044378 - 0x80044378 | size: 0x2b8 */
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BCF4;
extern u32 lbl_8047A4E0;
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E8;
/* fn_800492CC, fn_8011F5C8, fn_8011E778, fn_8011E760 already declared above */
#if 0
asm void fn_80044378(void) {
#include "src/game/scene_init_fn_80044378.inc"
}
#else
#pragma optimization_level 4
void fn_80044378(u8* ctx, u8* p) {
    u8* base;
    s16 val;
    u8* entry;
    s32 i;
    s32 byte_off;
    f32 fstep;
    f32 fy;
    u32 r25;
    u32 r26;
    u32 r27;
    u32 r3v;
    val = *(s16*)(p + 6);
    if (val == 0x12b2) {
        ctx[0x8b] = (u8)(lbl_8047BCA0 * *(f32*)((u8*)lbl_803A6818 + 0x54));
    } else {
        ctx[0x8b] = (u8)(lbl_8047BCA0 * *(f32*)((u8*)lbl_803A6818 + 0x4c));
    }
    val = *(s16*)(p + 6);
    switch (val) {
        case 0xd46:
        case 0x12b2:
        case 0x31d:
        case 0x31e:
            return;
        case 0x119b:
            if ((s8)fn_8004BDEC() != 1) return;
            if ((s8)fn_8004BDFC() < 1) return;
            fn_800492CC(ctx, p);
            return;
        case 0x76d:
            fn_80132A38(0x34, fn_8025FEE4(0));
            return;
    }
    entry = (u8*)lbl_802EF0A8 + val * 0x1c;
    fn_800FE38C(
        (s32)*(s16*)((u8*)lbl_802EF0A8 + 0x5712) - (s32)*(s16*)(entry + 2),
        (s32)*(s16*)((u8*)lbl_802EF0A8 + 0x5714) - (s32)*(s16*)(entry + 4),
        (s32)*(s16*)((u8*)lbl_802EF0A8 + 0x5716),
        (s32)*(s16*)((u8*)lbl_802EF0A8 + 0x5718));
    base = (u8*)lbl_803A6818;
    fy = *(f32*)(base + 0x30);
    byte_off = 0;
    fstep = lbl_8047BCF4;
    for (i = 0; i < (s32)*(u16*)&lbl_8047A4E8; i++) {
        if (i >= *(s32*)(base + 0x8) - 1 && i <= *(s32*)(base + 0xc) + 1) {
            r25 = lbl_8047A4E0;
            if (r25 != 0) {
                r27 = (u32)*(u16*)((u8*)lbl_8047A4E4 + byte_off);
                if (r27 >= 0x8000) {
                    r27 = r27 & 0x3fff;
                }
                fn_801240C4(r25, r27 & 0xFFFF, 0xa, gamedataGetStatus(0, 1));
                r26 = fn_8025FDDC(0, r27);
                r27 = fn_8025FD34(0, r27);
                fn_8011DFE0(r25, r26);
                fn_8011DF90(r25, r27);
                r3v = lbl_8047A4E0;
            } else {
                r3v = 0;
            }
            if (r3v != 0) {
                fn_8011F5C8();
                fn_8011E778();
                r3v = fn_800FA280(fn_8011E760());
            } else {
                r3v = 0;
            }
            if (r3v == 0) {
                r3v = fn_800FA280(1);
            }
            fn_80132A38(0x37, r3v);
            fn_800FB680(0, (s32)fy - 2, (s32)ctx[0x8b] | -0x100, 0xe7);
        }
        fy += fstep;
        byte_off += 2;
    }
    fn_800FE35C();
}
#endif

/* fn_80046168 - 0x80046168 | size: 0x1164 */
extern void fn_8011E15C(void);
extern void fn_8011CA60(void);
extern void fn_801666BC(void);
extern void fn_800F7AF0(void);
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BD1C;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCA0;
extern u32 lbl_8047BD18;
extern u32 lbl_8047A4EC;
extern u32 lbl_8047A4E4;
extern u32 lbl_8047A4E0;
extern f32 lbl_8047BCBC;
extern u32 lbl_8047BCFC;
extern u32 lbl_8047BD00;
#if 1
asm void fn_80046168(void) {
#include "src/game/scene_init_fn_80046168.inc"
}
#else
void fn_80046168(void) { /* TODO */ }
#endif

/* fn_800472CC - 0x800472CC | size: 0x114 */
extern f32 lbl_8047BCA0;
extern u32 lbl_8047A4EC;
extern f32 lbl_8047BCC4;
extern f32 lbl_8047BC94;
/* Call fn_800473E0 on 8 entries in lbl_802E52C8 (0x24 each), then fade
 * out entries below/above lbl_8047A4EC index. */
#if 0
asm void fn_800472CC(void) {
#include "src/game/scene_init_fn_800472CC.inc"
}
#else
#pragma optimization_level 4
void fn_800472CC(u8* ctx, u8* p) {
    u8* base = (u8*)lbl_803A6818;
    u8* tbl = (u8*)lbl_802E52C8;
    s32 i;
    s32 idx;

    ctx[0x8b] = (u8)(s32)(lbl_8047BCA0 * *(f32*)(base + 0x4c));
    for (i = 0; i < 8; i++) {
        fn_800473E0(tbl);
        tbl += 0x24;
    }
    /* Fade down entries from idx down to 0 */
    tbl = (u8*)lbl_802E52C8;
    idx = (s32)lbl_8047A4EC - 1;
    for (i = idx; i >= 0; i--) {
        f32 v = *(f32*)(tbl + i * 0x24 + 0x1c) - lbl_8047BCC4;
        *(f32*)(tbl + i * 0x24 + 0x1c) = v;
        if (v < lbl_8047BC94) {
            *(f32*)(tbl + i * 0x24 + 0x1c) = lbl_8047BC94;
        }
    }
    /* Fade down entries from 7 down to idx+1 */
    idx = (s32)lbl_8047A4EC;
    for (i = 7; (u32)idx < 8 && i >= (s32)idx; i--) {
        f32 v = *(f32*)(tbl + i * 0x24 + 0x1c) - lbl_8047BCC4;
        *(f32*)(tbl + i * 0x24 + 0x1c) = v;
        if (v < lbl_8047BC94) {
            *(f32*)(tbl + i * 0x24 + 0x1c) = lbl_8047BC94;
        }
    }
}
#endif

/* fn_800473E0 - 0x800473E0 | size: 0x4d4 */
extern void fn_80176228(void);
extern void fn_801760C4(void);
extern void fn_800D5648(void);
extern void fn_80176068(void);
extern u32 lbl_8047BCD8;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCBC;
extern u32 lbl_8047BD20;
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BD24;
extern u32 lbl_8047BD2C;
extern u32 lbl_8047BD28;
extern f32 lbl_8047BCA8;
#if 1
asm void fn_800473E0(u8* entry) {
#include "src/game/scene_init_fn_800473E0.inc"
}
#else
void fn_800473E0(void) { /* TODO */ }
#endif

/* fn_800478B4 - 0x800478B4 | size: 0x40c */
extern void fn_801DAC24(void);
extern void fn_800CE220(void);
extern void fn_800E064C(void);
extern void fn_800E03B4(void);
extern void fn_800E4598(void);
extern u32 lbl_8047BD30;
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BD34;
extern f32 lbl_8047BC94;
extern u8 lbl_802E5430[];
extern u32 lbl_8047BD18;
extern u8 lbl_802E5424[];
extern f32 lbl_8047BC98;
extern u32 lbl_8047BC9C;
#if 1
asm void fn_800478B4(void) {
#include "src/game/scene_init_fn_800478B4.inc"
}
#else
void fn_800478B4(void) { /* TODO */ }
#endif

/* fn_80047CC0 - 0x80047CC0 | size: 0x7e4 */
extern void fn_8025FA20(void);
extern void fn_800CD85C(void);
extern void fn_800E032C(void);
extern void fn_800E6BC8(void);
extern void fn_800EE0E8(void);
extern void fn_800EE150(void);
extern void fn_800EE3BC(void);
extern void fn_800EE828(void);
extern void fn_800E6B20(void);
extern u32 lbl_8047A4E0;
extern u32 lbl_8047BD38;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern u32 lbl_8047A4E4;
extern u32 lbl_8047BD3C;
extern u32 lbl_8047BD40;
extern u32 lbl_8047BD44;
extern u32 lbl_8047BD48;
extern u32 lbl_8047BD4C;
extern u32 lbl_8047BD30;
extern f64 lbl_8047BCB0;
extern f32 lbl_8047BC94;
extern u32 lbl_8047BD50;
extern u32 lbl_8047BD58;
extern u32 lbl_8047BD60;
extern u8 lbl_80478AC0[4];
extern u32 lbl_8047BD68;
extern u32 lbl_8047BD18;
extern u8 lbl_802E5418[];
extern u8 lbl_802E540C[];
extern f32 lbl_8047BC98;
extern u32 lbl_8047BC9C;
#if 1
asm void fn_80047CC0(void) {
#include "src/game/scene_init_fn_80047CC0.inc"
}
#else
void fn_80047CC0(void) { /* TODO */ }
#endif

/* fn_800484A4 - 0x800484A4 | size: 0x474 */
extern void fn_800E0518(void);
extern void fn_800E0370(void);
extern void fn_800DFF98(void);
extern void fn_800E0560(void);
extern u32 lbl_8047A4E0;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern u32 lbl_8047A4E4;
extern u32 lbl_8047BD3C;
extern u32 lbl_8047BD40;
extern u32 lbl_8047BD44;
extern u32 lbl_8047BD48;
extern u32 lbl_8047BD4C;
extern u32 lbl_8047BD30;
extern f64 lbl_8047BCB0;
extern u32 lbl_8047BD34;
extern f32 lbl_8047BC94;
extern u32 lbl_8047BD18;
extern u8 lbl_802E53F4[];
extern u8 lbl_802E5400[];
extern f32 lbl_8047BC98;
extern u32 lbl_8047BC9C;
#if 1
asm void fn_800484A4(void) {
#include "src/game/scene_init_fn_800484A4.inc"
}
#else
void fn_800484A4(void) { /* TODO */ }
#endif

/* fn_80048918 - 0x80048918 | size: 0x9b4 */
extern void fn_801096F8(void);
extern void fn_801CB954(void);
extern void fn_80113F48(void);
extern void fn_800F9318(void);
extern void fn_800E4014(void);
extern void fn_801CB9D8(void);
extern void fn_801CBA0C(void);
extern void fn_800EC9DC(void);
extern void GScameraGetPosition(void);
extern void GScameraGetRotation(void);
extern void fn_80176E0C(void);
extern u8 lbl_80267150[];
extern f32 lbl_8047BC94;
extern u32 lbl_8047BCF0;
extern u32 lbl_8047BD18;
extern u32 lbl_8047A4E0;
extern u32 lbl_804788C0;
extern f32 lbl_8047BCBC;
extern u32 lbl_8047A4E4;
extern u32 lbl_804788C4;
extern u32 lbl_8047BCCC;
extern f32 lbl_8047BC98;
extern u32 lbl_8047BC9C;
extern u32 lbl_8047A4E8;
#if 1
asm void fn_80048918(void) {
#include "src/game/scene_init_fn_80048918.inc"
}
#else
void fn_80048918(void) { /* TODO */ }
#endif

/* fn_800492CC - 0x800492CC | size: 0x2fc */
extern u32 lbl_804788C0;
extern s16 lbl_804788CC[2];
extern s16 lbl_804788C8[2];
extern s16 lbl_804788D0[2];
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCBC;
#if 0
asm void fn_800492CC(u8* a, u8* b) {
#include "src/game/scene_init_fn_800492CC.inc"
}
#else
#pragma optimization_level 4
void fn_800492CC(u8* a, u8* p) {
    u32 h2;
    u32 h1;
    s32 ival;
    s32 mode;

    h2 = 0;
    if ((s32)lbl_804788C0 == 0) return;
    mode = *(s32*)((u8*)lbl_803A6818 + 0x1c);
    if (mode == 0xc) return;
    switch (mode) {
        case 1:
        case 2:
            lbl_804788C8[0] = lbl_804788CC[0];
            lbl_804788C8[1] = lbl_804788CC[1];
            break;
        case 4:
            lbl_804788C8[0] = 0;
            lbl_804788C8[1] = 0;
            break;
        case 5:
            lbl_804788C8[0] = lbl_804788D0[0];
            lbl_804788C8[1] = lbl_804788D0[1];
            break;
    }
    if (mode == 5) {
        if (lbl_803A6818[0x214] != 0) {
            h1 = fn_80109934(lbl_803A6818 + 0x7c);
            h2 = fn_80109934(lbl_803A6818 + 0xc4);
        } else {
            h1 = 0;
            h2 = 0;
        }
    } else {
        if (lbl_803A6818[0x214] != 0) {
            h1 = fn_80109934(lbl_803A6818 + 0x7c);
        } else {
            h1 = 0;
        }
    }
    ival = (s32)(lbl_8047BCA0 * *(f32*)((u8*)lbl_803A6818 + 0x5c));
    if (h1 != 0) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, h1);
        fn_800D67BC(2);
        fn_800D61E4(lbl_804788C8[0], lbl_804788C8[1]);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, ival);
        fn_800D59B8(0, lbl_8047BC94, lbl_8047BC94);
        fn_800D61E4(
            (s16)(lbl_804788C8[0] + *(s16*)(p + 0x54)),
            (s16)(lbl_804788C8[1] + *(s16*)(p + 0x56)));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, ival);
        fn_800D59B8(0, lbl_8047BCBC, lbl_8047BCBC);
        fn_800D6728();
    }
    if (h2 != 0) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, h2);
        fn_800D67BC(2);
        fn_800D61E4(lbl_804788C8[0], lbl_804788C8[1]);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, ival);
        fn_800D59B8(0, lbl_8047BC94, lbl_8047BC94);
        fn_800D61E4(
            (s16)(lbl_804788C8[0] + *(s16*)(p + 0x54)),
            (s16)(lbl_804788C8[1] + *(s16*)(p + 0x56)));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, ival);
        fn_800D59B8(0, lbl_8047BCBC, lbl_8047BCBC);
        fn_800D6728();
    }
}
#endif

/* fn_8004A47C - 0x8004A47C | size: 0x32c */
extern void fn_801902E0(void);
extern u32 lbl_8047BD78;
extern u32 lbl_8047BD7C;
extern u32 lbl_8047BD80;
extern u32 lbl_8047BD84;
extern u8 lbl_8026719C[];
extern u8 lbl_802671B4[];
extern u32 lbl_8047BD88;
extern u32 lbl_8047BD8C;
extern u32 lbl_8047BD90;
extern u32 lbl_8047BD94;
#if 1
asm void fn_8004A47C(void) {
#include "src/game/scene_init_fn_8004A47C.inc"
}
#else
void fn_8004A47C(void) { /* TODO */ }
#endif

/* fn_8004A7A8 - 0x8004A7A8 | size: 0xad0 */
extern u8 lbl_803A6A60[];
extern u32 lbl_8047BDB0;
extern u32 lbl_8047BDCC;
extern u32 lbl_8047BDAC;
extern u32 lbl_804788D8;
extern u32 lbl_8047BDC8;
extern u32 lbl_8047BDA0;
extern u32 lbl_8047BDA8;
extern u32 lbl_8047BDD0;
extern u32 lbl_8047BDD4;
extern u32 lbl_8047BDB8;
#if 1
asm void fn_8004A7A8(void) {
#include "src/game/scene_init_fn_8004A7A8.inc"
}
#else
void fn_8004A7A8(void) { /* TODO */ }
#endif

/* fn_8004B278 - 0x8004B278 | size: 0x320 */
extern void fn_8005D8F8(void);
extern u32 lbl_8047BDC0;
extern u32 lbl_8047BDE8;
extern u32 lbl_8047BDA8;
extern u32 lbl_8047BDA0;
extern u32 lbl_8047BDAC;
extern u32 lbl_8047BDD8;
extern u32 lbl_8047BDDC;
extern u32 lbl_8047BDE0;
extern u32 lbl_8047BDE4;
#if 1
asm void fn_8004B278(void) {
#include "src/game/scene_init_fn_8004B278.inc"
}
#else
void fn_8004B278(void) { /* TODO */ }
#endif

/* fn_8004B598 - 0x8004B598 | size: 0x254 */
extern u8 lbl_80267190[];
extern u32 lbl_8047BDF0;
extern u32 lbl_8047BDAC;
#if 0
asm void fn_8004B598(void) {
#include "src/game/scene_init_fn_8004B598.inc"
}
#else
void fn_8004B598(s32 unused, u8* ctx, s32 mode) {
    u32 buf[3];
    u8 b;
    s32 sel;
    f32* arr;
    s16* tbl;

    buf[0] = ((u32*)lbl_80267190)[0];
    buf[1] = ((u32*)lbl_80267190)[1];
    buf[2] = ((u32*)lbl_80267190)[2];

    switch (mode) {
    case 0x6e7:
    case 0x75d:
    case 0x760:
    case 0x763:
        *(u32*)(lbl_803A6A60 + 0x44) = 0;
        break;
    case 0x6e8:
    case 0x75e:
    case 0x761:
    case 0x764:
        *(u32*)(lbl_803A6A60 + 0x44) = 1;
        break;
    case 0x6e9:
    case 0x75f:
    case 0x762:
    case 0x765:
        *(u32*)(lbl_803A6A60 + 0x44) = 2;
        break;
    }

    {
        u8 saved;
        b = lbl_803A6A60[0];
        if ((s8)b < 0) {
            saved = lbl_803A6A60[0x48];
        } else {
            saved = b;
            lbl_803A6A60[0x48] = b;
        }
        sel = (s8)saved;
    }

    switch (sel) {
    case 0:
        tbl = (s16*)(lbl_802EF0A8 + (*(u32*)(lbl_803A6A60 + 0x44) + 0x6e7) * 0x1c);
        break;
    case 1:
        tbl = (s16*)(lbl_802EF0A8 + (*(u32*)(lbl_803A6A60 + 0x44) + 0x75d) * 0x1c);
        break;
    case 2:
        tbl = (s16*)(lbl_802EF0A8 + (*(u32*)(lbl_803A6A60 + 0x44) + 0x760) * 0x1c);
        break;
    case 3:
        tbl = (s16*)(lbl_802EF0A8 + (*(u32*)(lbl_803A6A60 + 0x44) + 0x763) * 0x1c);
        break;
    default:
        tbl = (s16*)(lbl_802EF0A8 + 0xC144);
        break;
    }

    arr = (f32*)(lbl_803A6A60 + 0xc);
    arr[*(u32*)(lbl_803A6A60 + 0x44)] =
        arr[*(u32*)(lbl_803A6A60 + 0x44)]
        + *(f32*)(lbl_803A6A60 + 0x8) * (*(f32*)&lbl_8047BDF0 / *(f32*)&buf[*(u32*)(lbl_803A6A60 + 0x44)]);
    if (arr[*(u32*)(lbl_803A6A60 + 0x44)] > *(f32*)&lbl_8047BDF0) {
        arr[*(u32*)(lbl_803A6A60 + 0x44)] = arr[*(u32*)(lbl_803A6A60 + 0x44)] - *(f32*)&lbl_8047BDF0;
    }
    if (arr[*(u32*)(lbl_803A6A60 + 0x44)] < *(f32*)&lbl_8047BDAC) {
        arr[*(u32*)(lbl_803A6A60 + 0x44)] += *(f32*)&lbl_8047BDF0;
    }
    *(f32*)(ctx + 0x70) = arr[*(u32*)(lbl_803A6A60 + 0x44)];
    *(s16*)(ctx + 0x50) = tbl[1];
    *(s16*)(ctx + 0x52) = tbl[2];
}
#endif

/* fn_8004B7EC - 0x8004B7EC | size: 0x5cc */
extern void fn_801096E8(void);
extern u32 fn_800F92D4();
extern void fn_800E3CC8(void);
extern void fn_800ECCA8(void);
extern void fn_800ECA78(void);
extern void fn_800ECB74(void);
extern void fn_800DCAF0(void);
extern void fn_800DCADC(void);
extern void fn_800DCAD4(void);
extern void fn_800DCAB0(void);
extern void fn_800EC990(void);
extern void fn_800EC960(void);
extern void fn_800EC96C(void);
extern void fn_80176B48(void);
extern void fn_80044630(void);
extern void fn_8004C120(void);
extern void fn_8011288C(void);
extern u32 lbl_8047BDAC;
extern u32 lbl_8047BDA0;
extern u32 lbl_8047BDF8;
extern u32 lbl_8047BDF4;
extern u32 lbl_8047BDA8;
extern u8 lbl_8047A4FC[4];
extern u8 lbl_8047A4F8[4];
extern u32 lbl_8047BDFC;
extern u32 lbl_8047BE00;
#if 1
asm void fn_8004B7EC(void) {
#include "src/game/scene_init_fn_8004B7EC.inc"
}
#else
void fn_8004B7EC(void) { /* TODO */ }
#endif

/* fn_8004BDB8 - 0x8004BDB8 | size: 0x34 */
#if 0
asm void fn_8004BDB8(void) {
#include "src/game/scene_init_fn_8004BDB8.inc"
}
#else
#pragma optimization_level 4
void fn_8004BDB8(s8 a, s8 b) {
    if (a >= 0) {
        *(s8*)(lbl_803A6A60 + 1) = a;
    }
    if (b >= 0) {
        *(s8*)(lbl_803A6A60 + 1) = b;
    }
}
#endif

/* fn_8004BDEC - 0x8004BDEC | size: 0x10 */
#if 0
asm void fn_8004BDEC(void) {
#include "src/game/scene_init_fn_8004BDEC.inc"
}
#else
#pragma optimization_level 4
u8 fn_8004BDEC(void) {
    return lbl_803A6A60[0];
}
#endif

/* fn_8004BDFC - 0x8004BDFC | size: 0x10 */
#if 0
asm void fn_8004BDFC(void) {
#include "src/game/scene_init_fn_8004BDFC.inc"
}
#else
#pragma optimization_level 4
u8 fn_8004BDFC(void) {
    return lbl_803A6A60[1];
}
#endif

/* fn_8004BE40 - 0x8004BE40 | size: 0x50 */
extern s32 fn_801D1F7C(void);
extern u32 lbl_8047A500;
#if 0
asm s32 fn_8004BE40(s32 a) {
#include "src/game/scene_init_fn_8004BE40.inc"
}
#else
#pragma optimization_level 4
s32 fn_8004BE40(s32 a) {
    if (a < 0 || a >= fn_801D1F7C()) {
        return -1;
    }
    return (s32)*(u16*)((u8*)lbl_8047A500 + a * 2);
}
#endif

/* fn_8004BE90 - 0x8004BE90 | size: 0x90 */
extern u32 fn_801D1ACC(u32 a);
extern u32 mailGetReceiveNumber(u32 a);
#if 0
asm void fn_8004BE90(void) {
#include "src/game/scene_init_fn_8004BE90.inc"
}
#else
#pragma optimization_level 4
s32 fn_8004BE90(u16* a, u16* b) {
    u32 r30;
    u32 r31;
    u32 r29;
    u32 fa;
    s32 result;
    s32 ea;
    r31 = (u32)*a;
    r30 = (u32)*b;
    r29 = fn_800FA280(fn_801D1ACC(r31));
    fa = fn_800FA280(fn_801D1ACC(r30));
    result = GScharCmp(r29, fa);
    if (result != 0) {
        return result;
    }
    ea = mailGetReceiveNumber(r31);
    return (s32)(mailGetReceiveNumber(r30) - ea);
}
#endif

/* fn_8004BF20 - 0x8004BF20 | size: 0x90 */
extern u32 fn_801D1A88(u32 a);
#if 0
asm void fn_8004BF20(void) {
#include "src/game/scene_init_fn_8004BF20.inc"
}
#else
#pragma optimization_level 4
s32 fn_8004BF20(u16* a, u16* b) {
    u32 r30;
    u32 r31;
    u32 r29;
    u32 fa;
    s32 result;
    s32 ea;
    r31 = (u32)*a;
    r30 = (u32)*b;
    r29 = fn_800FA280(fn_801D1A88(r31));
    fa = fn_800FA280(fn_801D1A88(r30));
    result = GScharCmp(r29, fa);
    if (result != 0) {
        return result;
    }
    ea = mailGetReceiveNumber(r31);
    return (s32)(mailGetReceiveNumber(r30) - ea);
}
#endif

/* fn_8004BFB0 - 0x8004BFB0 | size: 0x170 */
extern s32 fn_801D1B4C(void);
extern u16 fn_801D1F0C(s32 a);
extern void fn_800CA620(u16* base, s32 count, u32 elemSize, void* cmpFunc);
extern u32 lbl_8047A500;
#if 0
asm void fn_8004BFB0(void) {
#include "src/game/scene_init_fn_8004BFB0.inc"
}
#else
#pragma optimization_level 4
void fn_8004BFB0(void) {
    u16* r28;
    u16* r30;
    s32 r31;
    s32 r29;
    r28 = (u16*)lbl_8047A500;
    switch (fn_801D1B4C()) {
    case 1:
        r30 = r28;
        r31 = 0;
        while (r31 < fn_801D1F7C()) {
            *r30 = (u16)fn_801D1F0C(r31);
            r31++;
            r30++;
        }
        break;
    case 2:
        r29 = fn_801D1F7C();
        r30 = r28;
        r31 = 0;
        while (r31 < r29) {
            *r30 = (u16)fn_801D1F0C(r31);
            r31++;
            r30++;
        }
        fn_800CA620(r28, r29, 2, (void*)fn_8004BF20);
        break;
    case 3:
        r31 = fn_801D1F7C();
        r30 = r28;
        r29 = 0;
        while (r29 < r31) {
            *r30 = (u16)fn_801D1F0C(r29);
            r29++;
            r30++;
        }
        fn_800CA620(r28, r31, 2, (void*)fn_8004BE90);
        break;
    default:
        r30 = r28;
        r29 = fn_801D1F7C() - 1;
        while (r29 >= 0) {
            *r30 = (u16)fn_801D1F0C(r29);
            r29--;
            r30++;
        }
        break;
    }
}
#endif

/* fn_8004C2D8 - 0x8004C2D8 | size: 0x94 */
#if 0
asm void fn_8004C2D8(void) {
#include "src/game/scene_init_fn_8004C2D8.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004C2D8(u8* ctx, u8* p) {
    u32 r31;
    s32 pages;
    r31 = (u32)ctx[0x8b] | 0xe66e0000u;
    pages = (fn_801D1F7C() + 9) / 10;
    if (pages <= 0) { pages = 1; }
    fn_80132A38(0x34, pages);
    fn_800FB680(*(s16*)(p + 0x54) - (s32)(fn_800FA444(0xca) >> 16), 0, (s32)r31, 0xca);
    return 0;
}
#endif

/* fn_8004C36C - 0x8004C36C | size: 0x78 */
#if 0
asm void fn_8004C36C(void) {
#include "src/game/scene_init_fn_8004C36C.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004C36C(u8* ctx, u8* p) {
    u32 r31;
    r31 = (u32)ctx[0x8b] | 0xe66e0000u;
    fn_80132A38(0x34, (s8)ctx[0x94] + 1);
    fn_800FB680(*(s16*)(p + 0x54) - (s32)(fn_800FA444(0xca) >> 16), 0, (s32)r31, 0xca);
    return 0;
}
#endif

/* fn_8004C3E4 - 0x8004C3E4 | size: 0xc0 */
extern u8 lbl_802672C8[];
#if 0
asm void fn_8004C3E4(void) {
#include "src/game/scene_init_fn_8004C3E4.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004C3E4(u8* ctx) {
    u32 buf[4];
    s32 idx;
    u32 r31;
    s32 r30;
    buf[0] = ((u32*)lbl_802672C8)[0];
    buf[1] = ((u32*)lbl_802672C8)[1];
    buf[2] = ((u32*)lbl_802672C8)[2];
    buf[3] = ((u32*)lbl_802672C8)[3];
    idx = fn_801D1B4C();
    if (idx < 0 || idx >= 4) {
        return 0;
    }
    r31 = buf[idx];
    r30 = (s32)((u32)ctx[0x8b] | 0xFFFFFF00u);
    fn_800FB680(0, 0, r30, r31);
    fn_800FB680((s32)(fn_800FA444(r31) >> 16), 0, r30, 0x36c1);
    return 0;
}
#endif

/* fn_8004C4A4 - 0x8004C4A4 | size: 0x10c */
extern f64 lbl_8047BE10;
extern f32 lbl_8047BE08;
extern f32 lbl_8047BE0C;
#if 0
asm void fn_8004C4A4(void) {
#include "src/game/scene_init_fn_8004C4A4.inc"
}
#else
#pragma fp_contract on
#pragma optimization_level 4
u32 fn_8004C4A4(u8* ctx, u8* p) {
    u8* r30;
    s32 pages;
    r30 = *(u8**)(ctx + 0x60);
    pages = (fn_801D1F7C() + 9) / 10;
    if (pages <= 1) {
        fn_80109220((u32)p, 0);
    } else {
        fn_80109220((u32)p, 1);
    }
    if (*(s16*)(p + 6) == 0x444) {
        s32 val = *(s32*)(r30 + 4);
        *(s16*)(p + 0x50) = (s16)(lbl_8047BE08 * *(f32*)(*(u32*)r30) + (f32)val);
    } else {
        s32 val = *(s32*)(r30 + 8);
        *(s16*)(p + 0x50) = (s16)(lbl_8047BE0C * *(f32*)(*(u32*)r30) + (f32)val);
    }
    return 0;
}
#pragma fp_contract off
#endif

/* fn_8004C5B0 - 0x8004C5B0 | size: 0x110 */
extern u32 fn_80103E68(u32 a);
extern u8 lbl_802671D0[];
#if 0
asm void fn_8004C5B0(void) {
#include "src/game/scene_init_fn_8004C5B0.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004C5B0(u8* ctx, u8* p) {
    u16 page;
    s32 idx;
    s32 val;
    s32* tbl;
    page = (u16)(fn_80103E68(0xa) >> 16);
    tbl = (s32*)lbl_802671D0;
    idx = 0;
    val = *(s16*)(p + 6);
    if (val == tbl[0]) { idx = 0; }
    else if (val == tbl[1]) { idx = 1; }
    else if (val == tbl[2]) { idx = 2; }
    else if (val == tbl[3]) { idx = 3; }
    else if (val == tbl[4]) { idx = 4; }
    else if (val == tbl[5]) { idx = 5; }
    else if (val == tbl[6]) { idx = 6; }
    else if (val == tbl[7]) { idx = 7; }
    else if (val == tbl[8]) { idx = 8; }
    else if (val == tbl[9]) { idx = 9; }
    else if (val == tbl[10]) { idx = 10; }
    else if (val == tbl[11]) { idx = 11; }
    else { idx = 12; }
    if (idx >= 12) { return 0; }
    if ((s8)(page & 0xff) == idx) {
        fn_80109220((u32)p, 1);
    } else {
        fn_80109220((u32)p, 0);
    }
    return 0;
}
#endif

/* fn_8004C6C0 - 0x8004C6C0 | size: 0x1ec */
extern u32 fn_801D1B78(void);
extern u8 lbl_802672A0[];
#if 0
asm void fn_8004C6C0(void) {
#include "src/game/scene_init_fn_8004C6C0.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004C6C0(u8* ctx, u8* p) {
    s32 tbl[10];
    u16 page;
    s32 r30;
    s32 val;
    s32* src;
    src = (s32*)lbl_802672A0;
    tbl[0] = src[0]; tbl[1] = src[1]; tbl[2] = src[2]; tbl[3] = src[3]; tbl[4] = src[4];
    tbl[5] = src[5]; tbl[6] = src[6]; tbl[7] = src[7]; tbl[8] = src[8]; tbl[9] = src[9];
    page = (u16)(fn_80103E68(0xa) >> 16);
    r30 = 0;
    val = *(s16*)(p + 6);
    if (val == tbl[0]) { r30 = 0; }
    else if (val == tbl[1]) { r30 = 1; }
    else if (val == tbl[2]) { r30 = 2; }
    else if (val == tbl[3]) { r30 = 3; }
    else if (val == tbl[4]) { r30 = 4; }
    else if (val == tbl[5]) { r30 = 5; }
    else if (val == tbl[6]) { r30 = 6; }
    else if (val == tbl[7]) { r30 = 7; }
    else if (val == tbl[8]) { r30 = 8; }
    else if (val == tbl[9]) { r30 = 9; }
    else { r30 = 10; }
    if (r30 >= 10) { return 0; }
    r30 = r30 + (s8)(page >> 8) * 10;
    if (fn_801D1A88(fn_8004BE40(r30)) != 0) {
        fn_80132A38(0x37, fn_800FA280());
        *(u32*)(p + 0x4c) = 0xe7;
    } else {
        *(u32*)(p + 0x4c) = 0;
    }
    if (fn_8004BE40(r30) >= 0) {
        if (fn_801D1B78() != 0) {
            p[0x64] = 0xff; p[0x65] = 0xff; p[0x66] = 0xff;
        } else {
            p[0x64] = 0xd5; p[0x65] = 0xaa; p[0x66] = 0x33;
        }
    }
    return 0;
}
#endif

/* fn_8004C8AC - 0x8004C8AC | size: 0x1ec */
extern u8 lbl_80267278[];
#if 0
asm void fn_8004C8AC(void) {
#include "src/game/scene_init_fn_8004C8AC.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004C8AC(u8* ctx, u8* p) {
    s32 tbl[10];
    u16 page;
    s32 r30;
    s32 val;
    s32* src;
    src = (s32*)lbl_80267278;
    tbl[0] = src[0]; tbl[1] = src[1]; tbl[2] = src[2]; tbl[3] = src[3]; tbl[4] = src[4];
    tbl[5] = src[5]; tbl[6] = src[6]; tbl[7] = src[7]; tbl[8] = src[8]; tbl[9] = src[9];
    page = (u16)(fn_80103E68(0xa) >> 16);
    r30 = 0;
    val = *(s16*)(p + 6);
    if (val == tbl[0]) { r30 = 0; }
    else if (val == tbl[1]) { r30 = 1; }
    else if (val == tbl[2]) { r30 = 2; }
    else if (val == tbl[3]) { r30 = 3; }
    else if (val == tbl[4]) { r30 = 4; }
    else if (val == tbl[5]) { r30 = 5; }
    else if (val == tbl[6]) { r30 = 6; }
    else if (val == tbl[7]) { r30 = 7; }
    else if (val == tbl[8]) { r30 = 8; }
    else if (val == tbl[9]) { r30 = 9; }
    else { r30 = 10; }
    if (r30 >= 10) { return 0; }
    r30 = r30 + (s8)(page >> 8) * 10;
    if (fn_801D1ACC(fn_8004BE40(r30)) != 0) {
        fn_80132A38(0x37, fn_800FA280());
        *(u32*)(p + 0x4c) = 0xe7;
    } else {
        *(u32*)(p + 0x4c) = 0;
    }
    if (fn_8004BE40(r30) >= 0) {
        if (fn_801D1B78() != 0) {
            p[0x64] = 0xff; p[0x65] = 0xff; p[0x66] = 0xff;
        } else {
            p[0x64] = 0xd5; p[0x65] = 0xaa; p[0x66] = 0x33;
        }
    }
    return 0;
}
#endif

/* fn_8004CA98 - 0x8004CA98 | size: 0x1a0 */
extern u32 fn_801D16F0();
extern u8 lbl_80267250[];
#if 0
asm void fn_8004CA98(void) {
#include "src/game/scene_init_fn_8004CA98.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004CA98(u8* ctx, u8* p) {
    s32 tbl[10];
    u16 page;
    s32 idx;
    s32 val;
    u8 r0;
    s32* src;
    src = (s32*)lbl_80267250;
    tbl[0] = src[0]; tbl[1] = src[1]; tbl[2] = src[2]; tbl[3] = src[3]; tbl[4] = src[4];
    tbl[5] = src[5]; tbl[6] = src[6]; tbl[7] = src[7]; tbl[8] = src[8]; tbl[9] = src[9];
    page = (u16)(fn_80103E68(0xa) >> 16);
    idx = 0;
    val = *(s16*)(p + 6);
    if (val == tbl[0]) { idx = 0; }
    else if (val == tbl[1]) { idx = 1; }
    else if (val == tbl[2]) { idx = 2; }
    else if (val == tbl[3]) { idx = 3; }
    else if (val == tbl[4]) { idx = 4; }
    else if (val == tbl[5]) { idx = 5; }
    else if (val == tbl[6]) { idx = 6; }
    else if (val == tbl[7]) { idx = 7; }
    else if (val == tbl[8]) { idx = 8; }
    else if (val == tbl[9]) { idx = 9; }
    else { idx = 10; }
    if (idx >= 10) { return 0; }
    idx = idx + (s8)(page >> 8) * 10;
    if (fn_8004BE40(idx) < 0) {
        r0 = 0;
    } else if (fn_801D16F0() != 0) {
        r0 = 1;
    } else {
        r0 = 0;
    }
    fn_80109220((u32)p, (u32)r0);
    return 0;
}
#endif

/* fn_8004CC38 - 0x8004CC38 | size: 0x1a0 */
extern u8 lbl_80267228[];
#if 0
asm void fn_8004CC38(void) {
#include "src/game/scene_init_fn_8004CC38.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004CC38(u8* ctx, u8* p) {
    s32 tbl[10];
    u16 page;
    s32 idx;
    s32 val;
    u8 r0;
    s32* src;
    src = (s32*)lbl_80267228;
    tbl[0] = src[0]; tbl[1] = src[1]; tbl[2] = src[2]; tbl[3] = src[3]; tbl[4] = src[4];
    tbl[5] = src[5]; tbl[6] = src[6]; tbl[7] = src[7]; tbl[8] = src[8]; tbl[9] = src[9];
    page = (u16)(fn_80103E68(0xa) >> 16);
    idx = 0;
    val = *(s16*)(p + 6);
    if (val == tbl[0]) { idx = 0; }
    else if (val == tbl[1]) { idx = 1; }
    else if (val == tbl[2]) { idx = 2; }
    else if (val == tbl[3]) { idx = 3; }
    else if (val == tbl[4]) { idx = 4; }
    else if (val == tbl[5]) { idx = 5; }
    else if (val == tbl[6]) { idx = 6; }
    else if (val == tbl[7]) { idx = 7; }
    else if (val == tbl[8]) { idx = 8; }
    else if (val == tbl[9]) { idx = 9; }
    else { idx = 10; }
    if (idx >= 10) { return 0; }
    idx = idx + (s8)(page >> 8) * 10;
    if (fn_8004BE40(idx) < 0) {
        r0 = 0;
    } else if (fn_801D1B78() != 0) {
        r0 = 1;
    } else {
        r0 = 0;
    }
    fn_80109220((u32)p, (u32)r0);
    return 0;
}
#endif

/* fn_8004CDD8 - 0x8004CDD8 | size: 0x1a0 */
extern u8 lbl_80267200[];
#if 0
asm void fn_8004CDD8(void) {
#include "src/game/scene_init_fn_8004CDD8.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004CDD8(u8* ctx, u8* p) {
    s32 tbl[10];
    u16 page;
    s32 idx;
    s32 val;
    u8 r0;
    s32* src;
    src = (s32*)lbl_80267200;
    tbl[0] = src[0]; tbl[1] = src[1]; tbl[2] = src[2]; tbl[3] = src[3]; tbl[4] = src[4];
    tbl[5] = src[5]; tbl[6] = src[6]; tbl[7] = src[7]; tbl[8] = src[8]; tbl[9] = src[9];
    page = (u16)(fn_80103E68(0xa) >> 16);
    idx = 0;
    val = *(s16*)(p + 6);
    if (val == tbl[0]) { idx = 0; }
    else if (val == tbl[1]) { idx = 1; }
    else if (val == tbl[2]) { idx = 2; }
    else if (val == tbl[3]) { idx = 3; }
    else if (val == tbl[4]) { idx = 4; }
    else if (val == tbl[5]) { idx = 5; }
    else if (val == tbl[6]) { idx = 6; }
    else if (val == tbl[7]) { idx = 7; }
    else if (val == tbl[8]) { idx = 8; }
    else if (val == tbl[9]) { idx = 9; }
    else { idx = 10; }
    if (idx >= 10) { return 0; }
    idx = idx + (s8)(page >> 8) * 10;
    if (fn_8004BE40(idx) < 0) {
        r0 = 0;
    } else if (fn_801D1B78() == 0) {
        r0 = 1;
    } else {
        r0 = 0;
    }
    fn_80109220((u32)p, (u32)r0);
    return 0;
}
#endif

/* fn_8004CF78 - 0x8004CF78 | size: 0x2f4 */
extern void fn_80103EAC(u32 sceneId, u16* pageData);
#if 0
asm void fn_8004CF78(void) {
#include "src/game/scene_init_fn_8004CF78.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_8004CF78(u8* p) {
    u16 packed;
    u16 tmp;
    u8* ctx;
    s32 r3;

    ctx = fn_80105624();
    fn_80103E68(0xa);
    packed = *(u16*)(p + 0x94);

    if ((*(u16*)(ctx + 6) & 2) != 0) {
        ((u8*)&packed)[1] = (u8)(((u8*)&packed)[1] + 1);
        if ((s8)((u8*)&packed)[1] > 0xb) {
            ((u8*)&packed)[1] = 0;
        }
        r3 = fn_801D1F7C() - (s8)((u8*)&packed)[0] * 10;
        if (r3 > 0xa) {
            r3 = 0xa;
        } else if (r3 < 0) {
            r3 = 0;
        }
        if ((s8)((u8*)&packed)[1] < 0xa && (s8)((u8*)&packed)[1] >= r3) {
            ((u8*)&packed)[1] = 0xa;
        }
    }
    if ((*(u16*)(ctx + 6) & 1) != 0) {
        ((u8*)&packed)[1] = (u8)(((u8*)&packed)[1] - 1);
        if ((s8)((u8*)&packed)[1] < 0) {
            ((u8*)&packed)[1] = 0xb;
        }
        r3 = fn_801D1F7C() - (s8)((u8*)&packed)[0] * 10;
        if (r3 > 0xa) {
            r3 = 0xa;
        } else if (r3 < 0) {
            r3 = 0;
        }
        if ((s8)((u8*)&packed)[1] < 0xa && (s8)((u8*)&packed)[1] >= r3) {
            if (r3 > 0) {
                ((u8*)&packed)[1] = (u8)(r3 - 1);
            } else {
                ((u8*)&packed)[1] = 0xb;
            }
        }
    }
    if ((*(u16*)(ctx + 6) & 8) != 0) {
        if ((s8)((u8*)&packed)[1] < 0xa) {
            r3 = fn_801D1F7C();
            ((u8*)&packed)[0] = (u8)(((u8*)&packed)[0] + 1);
            if ((s8)((u8*)&packed)[0] >= (r3 + 9) / 10) {
                ((u8*)&packed)[0] = 0;
            }
            r3 = fn_801D1F7C() - (s8)((u8*)&packed)[0] * 10;
            if (r3 > 0xa) {
                r3 = 0xa;
            } else if (r3 < 0) {
                r3 = 0;
            }
            if ((s8)((u8*)&packed)[1] >= r3) {
                ((u8*)&packed)[1] = (u8)(r3 - 1);
            }
        } else {
            ((u8*)&packed)[1] = 0xb;
        }
    }
    if ((*(u16*)(ctx + 6) & 4) != 0) {
        if ((s8)((u8*)&packed)[1] < 0xa) {
            ((u8*)&packed)[0] = (u8)(((u8*)&packed)[0] - 1);
            if ((s8)((u8*)&packed)[0] < 0) {
                r3 = fn_801D1F7C();
                ((u8*)&packed)[0] = (u8)((r3 + 9) / 10 - 1);
            }
            r3 = fn_801D1F7C() - (s8)((u8*)&packed)[0] * 10;
            if (r3 > 0xa) {
                r3 = 0xa;
            } else if (r3 < 0) {
                r3 = 0;
            }
            if ((s8)((u8*)&packed)[1] >= r3) {
                ((u8*)&packed)[1] = (u8)(r3 - 1);
            }
        } else {
            ((u8*)&packed)[1] = 0xa;
        }
    }

    tmp = packed;
    fn_80103EAC(0xa, &tmp);
    *(u16*)(p + 0x94) = packed;
    return 0;
}
#pragma pop
#endif

/* fn_8004D26C - 0x8004D26C | size: 0xe0 */
extern f32 lbl_8047BE18;
extern f32 lbl_8047BE1C;
#if 0
asm void fn_8004D26C(void) {
#include "src/game/scene_init_fn_8004D26C.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004D26C(u8* p) {
    u8* r4;
    r4 = *(u8**)(p + 0x60);
    switch (*(s8*)(p + 1)) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c2);
            p[2] = 1;
        }
        break;
    case 2:
        {
            f32 f0;
            f32 f1 = lbl_8047BE1C;
            f32* fp = (f32*)(*(u32*)r4);
            f0 = *fp + lbl_8047BE18;
            *fp = f0;
            if (f0 >= f1) {
                *fp = *fp - f1;
            }
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c6);
            p[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

/* fn_8004D34C - 0x8004D34C | size: 0x244 */
extern void fn_801D1B10(void);
extern void fn_801D16C4(void);
extern void fn_801D167C(void);
extern u32 lbl_804788E0;
extern u32 lbl_8047A50C;
extern u32 lbl_8047A508;
extern f32 lbl_8047BE20;
extern f32 lbl_8047A510;
#if 1
asm void fn_8004D34C(void) {
#include "src/game/scene_init_fn_8004D34C.inc"
}
#else
void fn_8004D34C(void) { /* TODO */ }
#endif

/* fn_8004D590 - 0x8004D590 | size: 0x5c */
#if 0
asm void fn_8004D590(void) {
#include "src/game/scene_init_fn_8004D590.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D590(u8* p, u8* q) {
    u32 v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    fn_8004BE40((s32)v);
    if (fn_801D16F0() != 0) {
        *(u32*)(q + 0x4c) = 0x36b9;
    } else {
        *(u32*)(q + 0x4c) = 0;
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D5EC - 0x8004D5EC | size: 0x60 */
#if 0
asm void fn_8004D5EC(void) {
#include "src/game/scene_init_fn_8004D5EC.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D5EC(u8* p, u32 q) {
    u32 v;
    u8 val;

    v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    fn_8004BE40((s32)v);
    if (fn_801D16F0() != 0) {
        val = 1;
    } else {
        val = 0;
    }
    fn_80109220(q, val);
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D64C - 0x8004D64C | size: 0x60 */
#if 0
asm void fn_8004D64C(void) {
#include "src/game/scene_init_fn_8004D64C.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D64C(u8* p, u32 q) {
    u32 v;
    u8 val;

    v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    fn_8004BE40((s32)v);
    if (fn_801D16F0() != 0) {
        val = 1;
    } else {
        val = 0;
    }
    fn_80109220(q, val);
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D6AC - 0x8004D6AC | size: 0x44 */
extern u32 fn_801D1A44(void);
#if 0
asm void fn_8004D6AC(void) {
#include "src/game/scene_init_fn_8004D6AC.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D6AC(u8* p, u8* q) {
    u32 v;

    v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    fn_8004BE40((s32)v);
    *(u32*)(q + 0x4c) = fn_801D1A44();
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D6F0 - 0x8004D6F0 | size: 0x70 */
#if 0
asm void fn_8004D6F0(void) {
#include "src/game/scene_init_fn_8004D6F0.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D6F0(u8* p, u8* q) {
    u32 v;
    u32 result;
    u32 fa;

    v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    result = fn_801D1A88(fn_8004BE40((s32)v));
    if (result != 0) {
        fa = fn_800FA280();
        fn_80132A38(0x37, fa);
        *(u32*)(q + 0x4c) = 0xe7;
    } else {
        *(u32*)(q + 0x4c) = 0;
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D760 - 0x8004D760 | size: 0x70 */
#if 0
asm void fn_8004D760(void) {
#include "src/game/scene_init_fn_8004D760.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D760(u8* p, u8* q) {
    u32 v;
    u32 result;
    u32 fa;

    v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    result = fn_801D1ACC(fn_8004BE40((s32)v));
    if (result != 0) {
        fa = fn_800FA280();
        fn_80132A38(0x37, fa);
        *(u32*)(q + 0x4c) = 0xe7;
    } else {
        *(u32*)(q + 0x4c) = 0;
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D7D0 - 0x8004D7D0 | size: 0xec */
extern void fn_801D1C20(s32 a);
extern void fn_801D228C(u16 a);
#if 0
asm void fn_8004D7D0(void) {
#include "src/game/scene_init_fn_8004D7D0.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D7D0(u8* p) {
    u32 r30;
    u8* ctx;
    s32 r29;
    s32 orig;

    r30 = *(u32*)(p + 0x60);
    ctx = fn_80105624();
    r29 = *(s32*)*(u32*)r30;
    orig = r29;
    if (*(u16*)(ctx + 6) & 0x2) {
        r29 = r29 + 1;
        if (r29 >= fn_801D1F7C()) {
            r29 = 0;
        }
    }
    if (*(u16*)(ctx + 6) & 0x1) {
        r29 = r29 - 1;
        if (r29 < 0) {
            r29 = fn_801D1F7C() - 1;
        }
    }
    if (r29 != orig) {
        fn_80166A50(0x23, 0, 0xff, 0);
        *(s32*)*(u32*)r30 = r29;
    }
    r29 = fn_8004BE40(r29);
    if ((s32)fn_801D1B78() == 0) {
        fn_801D1C20(r29);
        fn_801D228C((u16)r29);
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D8BC - 0x8004D8BC | size: 0x6c */
#if 0
asm void fn_8004D8BC(void) {
#include "src/game/scene_init_fn_8004D8BC.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma optimization_level 4
void fn_8004D8BC(u8* p) {
    u32 v;
    u8* ctx;

    ctx = fn_80105624();
    v = *(u32*)(p + 0x60);
    v = *(u32*)v;
    v = *(u32*)v;
    fn_8004BE40((s32)v);
    if (fn_801D16F0() != 0 || (*(u16*)ctx & 0x10) == 0) {
        fn_80102ED4(p);
    }
}
#pragma pop
#endif

/* fn_8004D928 - 0x8004D928 | size: 0x98 */
#if 0
asm void fn_8004D928(void) {
#include "src/game/scene_init_fn_8004D928.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D928(u8* p) {
    switch (*(s8*)(p + 1)) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c2);
            *(u8*)(p + 2) = 1;
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c6);
            *(u8*)(p + 2) = 1;
        }
        break;
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004D9C0 - 0x8004D9C0 | size: 0xa4 */
extern u32 lbl_8047A518;
#if 0
asm void fn_8004D9C0(void) {
#include "src/game/scene_init_fn_8004D9C0.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004D9C0(u32 a) {
    u32 pptr;
    s32 r30;
    s32 result;
    lbl_8047A518 = a;
    pptr = (u32)&lbl_8047A518;
    for (;;) {
        result = fn_801026A4(0x74, (u32)fn_801046B8(), 0, 0, 1, 1, (u32)&pptr);
        if (result == -1) { break; }
        r30 = fn_8004BE40((s32)lbl_8047A518);
        if (fn_801D16F0() == 0) { continue; }
        fn_8004E9C0(r30);
    }
    fn_80102510(0x74);
    menuCloseSync(0x74, 1);
    return lbl_8047A518;
}
#pragma scheduling off
#endif

/* fn_8004DA64 - 0x8004DA64 | size: 0xd0 */
extern u8 lbl_802672D8[];
#if 0
asm void fn_8004DA64(void) {
#include "src/game/scene_init_fn_8004DA64.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004DA64(u8* p, u8* q) {
    u32 r5;
    s32* tbl;
    s32 val;
    s32 idx;
    u8 result;

    r5 = *(u32*)(p + 0x60);
    tbl = (s32*)lbl_802672D8;
    if (*(u32*)r5 != 0) {
        result = 0;
    } else {
        idx = 0;
        val = *(s16*)(q + 6);
        if (val == tbl[0]) {
            idx = 0;
        } else if (val == tbl[1]) {
            idx = 1;
        } else if (val == tbl[2]) {
            idx = 2;
        } else if (val == tbl[3]) {
            idx = 3;
        } else if (val == tbl[4]) {
            idx = 4;
        } else {
            idx = 5;
        }
        if (idx >= 5) {
            return 0;
        }
        if (*(s8*)(p + 0x95) == idx) {
            result = 1;
        } else {
            result = 0;
        }
    }
    fn_80109220((u32)q, result);
    return 0;
}
#endif

/* fn_8004DB34 - 0x8004DB34 | size: 0x4c */
#if 0
asm void fn_8004DB34(void) {
#include "src/game/scene_init_fn_8004DB34.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004DB34(u8* p, u8* q) {
    if (*(s32*)(*(u32*)(p + 0x60)) != 0) {
        fn_80109220((u32)q, 0);
    } else {
        fn_80109220((u32)q, 1);
    }
    return 0;
}
#endif

/* fn_8004DB80 - 0x8004DB80 | size: 0x98 */
#if 0
asm void fn_8004DB80(void) {
#include "src/game/scene_init_fn_8004DB80.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004DB80(u8* p) {
    switch (*(s8*)(p + 1)) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c2);
            *(u8*)(p + 2) = 1;
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c6);
            *(u8*)(p + 2) = 1;
        }
        break;
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_8004DC18 - 0x8004DC18 | size: 0xa8 */
#if 0
asm s32 fn_8004DC18(u32 a) {
#include "src/game/scene_init_fn_8004DC18.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
s32 fn_8004DC18(u32 a) {
    u32 changed;
    u32 saved;
    s32 result;

    saved = a;
    changed = 0;
    result = fn_801026A4(0x75, (u32)fn_801046B8(), (u32)&saved, 0, 1, 1, (u32)&changed);
    if (result != -1) {
        if (result != (s32)saved) {
            changed = 1;
        }
    }
    fn_80102510(0x75);
    menuCloseSync(0x75, 1);
    if (result < 0 || result >= 4) {
        return -1;
    }
    return result;
}
#pragma scheduling off
#endif

/* fn_8004DCC0 - 0x8004DCC0 | size: 0x100 */
extern u32 fn_801D1620(u8 a);
extern u8 lbl_802672F0[];
#if 0
asm void fn_8004DCC0(void) {
#include "src/game/scene_init_fn_8004DCC0.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004DCC0(u8* p, u8* q) {
    s32 tbl[11];
    s32 i;
    s32 val;
    u32 result;
    s32* src;

    src = (s32*)lbl_802672F0;
    i = 0;
    tbl[0] = src[0];
    tbl[1] = src[1];
    tbl[2] = src[2];
    tbl[3] = src[3];
    tbl[4] = src[4];
    tbl[5] = src[5];
    tbl[6] = src[6];
    tbl[7] = src[7];
    tbl[8] = src[8];
    tbl[9] = src[9];
    tbl[10] = src[10];
    val = *(s16*)(q + 6);
    for (i = 0; i < 11; i++) {
        if (val == tbl[i]) {
            break;
        }
    }
    if (i >= 11) {
        return 0;
    }
    result = fn_801D1620((u8)i);
    if (result != 0) {
        *(u32*)(q + 0x4c) = result;
    } else {
        *(u32*)(q + 0x4c) = 0x36cd;
    }
    return 0;
}
#endif

/* fn_8004DDC0 - 0x8004DDC0 | size: 0x174 */
extern u32 fn_801D1650(u32 a);
extern void fn_80166B18(void);
extern void fn_801654E0(u32 a, u32 b, u32 c);
extern void fn_80166B3C(u32 a, u32 b, u32 c);
extern s32 fn_801D1618(void);
extern void fn_801669E4(u32 a, u32 b, u32 c);
extern s32 lbl_8047A520;
extern u8 lbl_8047A524;
extern u32 lbl_8047A52C;
extern u32 lbl_8047A528;
#if 0
asm void fn_8004DDC0(void) {
#include "src/game/scene_init_fn_8004DDC0.inc"
}
#else
#pragma peephole off
#pragma scheduling on
#pragma optimization_level 4
u32 fn_8004DDC0(u8* ctx) {
    u8* r30;
    u32 r29;
    r30 = fn_80105624();
    if (lbl_8047A520 != 0) {
        if (fn_801D1650((u32)lbl_8047A524) != 0) {
            fn_801666BC();
        }
        if (fn_801D1650((u32)lbl_8047A524) != 0) {
            fn_80166B18();
        }
        lbl_8047A524 = ctx[0x95];
        r29 = fn_801D1650((u32)ctx[0x95]);
        if (r29 != 0) {
            fn_801654E0(r29, lbl_8047A52C, 0x10000);
            fn_80166B3C(r29, 0, 0x408);
            fn_80166A50(r29, 0, 0xff, 0);
            lbl_8047A528 = 0;
        }
        lbl_8047A520 = 0;
    }
    if (*(u16*)(r30 + 4) & 2) {
        s32 limit;
        s8 cur;
        limit = fn_801D1618();
        cur = (s8)(ctx[0x95] + 1);
        *(s8*)(ctx + 0x95) = cur;
        if (cur >= limit + 1) {
            *(s8*)(ctx + 0x95) = (s8)(limit + 1 - 1);
        }
    }
    if (*(u16*)(r30 + 4) & 1) {
        s32 cur;
        cur = (s32)ctx[0x95] - 1;
        *(s8*)(ctx + 0x95) = (s8)cur;
        if ((s8)cur < 0) {
            ctx[0x95] = 0;
        }
    }
    if (lbl_8047A524 != (s8)ctx[0x95]) {
        r29 = fn_801D1650((u32)lbl_8047A524);
        if (r29 != 0) {
            fn_801669E4(r29, 0, 0);
        }
        lbl_8047A520 = 1;
    }
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_8004DF34 - 0x8004DF34 | size: 0x98 */
#if 0
asm void fn_8004DF34(void) {
#include "src/game/scene_init_fn_8004DF34.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_8004DF34(u8* p) {
    switch (*(s8*)(p + 1)) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c2);
            *(u8*)(p + 2) = 1;
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x1c6);
            *(u8*)(p + 2) = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
#endif

/* fn_8004DFCC - 0x8004DFCC | size: 0x178 */
extern u32 fn_8016557C(void);
extern u32 fn_800F9418(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800F9378(u32 a, u32 b, u32 c, u32 d);
extern void fn_800F9210(u32 a, u32 b);
extern void fn_80165548(u32 a);
extern u32 lbl_8047A52C;
extern u32 lbl_8047A528;
#if 0
asm u8 fn_8004DFCC(u8 a) {
#include "src/game/scene_init_fn_8004DFCC.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u8 fn_8004DFCC(u8 a) {
    u32 buf;
    u32 r30;
    u32 snd;
    s32 result;

    buf = (u32)a;
    r30 = fn_8016557C();
    lbl_8047A52C = fn_800F9418(0x10000, 0x20, 0, 0x408, 0);
    fn_800F9378(lbl_8047A52C, 0, 0x408, 0);
    lbl_8047A524 = (u32)a;
    snd = fn_801D1650((u32)a);
    if (snd != 0) {
        fn_801654E0(snd, lbl_8047A52C, 0x10000);
        fn_80166B3C(snd, 0, 0x408);
        fn_80166A50(snd, 0, 0xff, 0);
        lbl_8047A528 = 0;
    }
    result = fn_801026A4(0x76, (u32)fn_801046B8(), (u32)&buf, 0, 1, 0);
    if (result < 0 || result >= fn_801D1618()) {
        result = 0xff;
    } else {
        result = result & 0xff;
    }
    fn_80102510(0x76);
    menuCloseSync(0x76, 1);
    snd = fn_801D1650(lbl_8047A524);
    if (snd != 0) {
        fn_801669E4(snd, 0, 0);
    }
    snd = fn_801D1650(lbl_8047A524);
    if (snd != 0) {
        fn_801666BC();
    }
    snd = fn_801D1650(lbl_8047A524);
    if (snd != 0) {
        fn_80166B18();
    }
    fn_800F9210(0, 0x408);
    fn_80165548(r30);
    return (u8)result;
}
#pragma pop
#endif

/* fn_8004E144 - 0x8004E144 | size: 0x3c */
extern f64 lbl_8047BE28;
extern f64 lbl_8047BE30;
#if 0
asm void fn_8004E144(void) {
#include "src/game/scene_init_fn_8004E144.inc"
}
#else
#pragma optimization_level 4
u32 fn_8004E144(u32 unused, u8* p) {
    f32 f0;
    f0 = (f32)(*(f32*)(p + 0x70) + lbl_8047BE28);
    *(f32*)(p + 0x70) = f0;
    if (f0 >= (f32)lbl_8047BE30) {
        *(f32*)(p + 0x70) = (f32)(f0 - (f32)lbl_8047BE30);
    }
    return 0;
}
#endif

/* fn_8004E180 - 0x8004E180 | size: 0x160 */
extern s32 fn_8017B448();
extern u32 fn_8017B4BC();
extern u32 fn_8017B5A4();
#if 0
asm void fn_8004E180(void) {
#include "src/game/scene_init_fn_8004E180.inc"
}
#else
s32 fn_8004E180(u8* ctx, u8* p) {
    s32 curIdx;
    u8* subctx;
    u32 obj;
    s32 count;
    s32 total;
    s32 i;
    s16 widthBig;
    s16 widthSmall;
    s32 x;

    subctx = *(u8**)(ctx + 0x60);
    count = *(u32*)(subctx + 4);
    curIdx = *(s32*)(*(u32*)(subctx + 8));
    if (fn_8017B2CC(fn_801D16F0(count)) != 1) {
        obj = fn_801D16F0(count);
        total = fn_8017B448();
        count = 0;
        i = 0;
        while (i < total) {
            fn_8017B4BC(obj, i);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            i++;
        }
    } else {
        count = -1;
    }
    if (count <= 0) {
        return 0;
    }
    widthBig = (s16)(fn_800FA444(0x36ce) >> 16);
    widthSmall = (s16)(fn_800FA444(0x36cf) >> 16);
    x = *(s16*)(p + 0x54) / 2 - (widthBig + widthSmall * (count - 1)) / 2;
    for (i = 0; i < count; i++) {
        if (i == curIdx) {
            fn_800FB680(x, 0, (s32)0xe6aa00ff, 0x36ce);
            x += widthBig;
        } else {
            fn_800FB680(x, 0, (s32)0xaaaaaaffU, 0x36cf);
            x += widthSmall;
        }
    }
    return 0;
}
#endif

/* fn_8004E2E0 - 0x8004E2E0 | size: 0x160 */
extern f64 lbl_8047BE40;
extern f32 lbl_8047BE38;
extern f32 lbl_8047BE3C;
#if 0
asm void fn_8004E2E0(void) {
#include "src/game/scene_init_fn_8004E2E0.inc"
}
#else
s32 fn_8004E2E0(u8* ctx, u8* p) {
    u8* subctx;
    u32 obj;
    s32 count;
    s32 i;
    s32 total;

    subctx = *(u8**)(ctx + 0x60);
    count = *(u32*)(subctx + 4);
    if (fn_8017B2CC(fn_801D16F0(count)) != 1) {
        obj = fn_801D16F0(count);
        total = fn_8017B448();
        count = 0;
        i = 0;
        while (i < total) {
            fn_8017B4BC(obj, i);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            i++;
        }
    } else {
        count = -1;
    }
    if (count < 2) {
        fn_80109220((u32)p, 0);
    } else {
        fn_80109220((u32)p, 1);
    }
    if (*(s16*)(p + 6) == 0x507) {
        *(s16*)(p + 0x50) = (s16)(lbl_8047BE38 * *(f32*)(*(u32*)subctx) + (f32)(s32)*(u32*)(subctx + 0xc));
    } else {
        *(s16*)(p + 0x50) = (s16)(lbl_8047BE3C * *(f32*)(*(u32*)subctx) + (f32)(s32)*(u32*)(subctx + 0x10));
    }
    return 0;
}
#endif

/* fn_8004E440 - 0x8004E440 | size: 0xd0 */
#if 0
asm void fn_8004E440(void) {
#include "src/game/scene_init_fn_8004E440.inc"
}
#else
s32 fn_8004E440(u8* ctx, u8* p) {
    s32 total;
    s32 i;
    s32 count;
    u32 obj;
    u32 ptr;

    ptr = *(u32*)(*(u32*)(ctx + 0x60) + 4);
    if (fn_8017B2CC(fn_801D16F0(ptr)) == 1) {
        count = -1;
    } else {
        obj = fn_801D16F0(ptr);
        total = fn_8017B448();
        count = 0;
        i = 0;
        while (i < total) {
            fn_8017B4BC(obj, i);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            i++;
        }
    }
    if (count <= 0) {
        fn_80109220((u32)p, 0);
    } else {
        fn_80109220((u32)p, 1);
    }
    return 0;
}
#endif

/* fn_8004E510 - 0x8004E510 | size: 0x280 */
extern u8 fn_8017B07C();
extern f32 lbl_8047BE48;
extern f32 lbl_8047BE4C;
#if 0
asm void fn_8004E510(void) {
#include "src/game/scene_init_fn_8004E510.inc"
}
#else
s32 fn_8004E510(u8* ctx, u8* p) {
    u8* subctx;
    u32 obj;
    u32 obj2;
    u32 obj3;
    u32 r25;
    s32 count;
    s32 count2;
    s32 i;
    s32 total;
    s32 total2;
    u32 r31;
    u32 r23;
    u32 ptr;
    s32 ok;

    subctx = *(u8**)(ctx + 0x60);
    ptr = *(u32*)(subctx + 4);
    if (fn_8017B2CC(fn_801D16F0(ptr)) == 1) {
        ok = 0;
    } else {
        ok = 1;
    }
    if (ok != 0) {
        obj = fn_801D16F0(ptr);
        total = fn_8017B448();
        count = 0;
        i = 0;
        while (i < total) {
            fn_8017B4BC(obj, i);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            i++;
        }
    } else {
        count = -1;
    }
    if (count <= 0) {
        return 0;
    }
    r25 = *(u32*)(*(u32*)(subctx + 8));
    ptr = *(u32*)(subctx + 4);
    if (fn_8017B2CC(fn_801D16F0(ptr)) == 1) {
        ok = 0;
    } else {
        ok = 1;
    }
    if (ok != 0) {
        obj2 = fn_801D16F0(ptr);
        total2 = fn_8017B448();
        count2 = 0;
        i = 0;
        while (i < total2) {
            fn_8017B4BC(obj2, i);
            if (fn_8017B5A4() == 9) {
                count2++;
            }
            i++;
        }
    } else {
        count2 = -1;
    }
    if (count2 <= (s32)r25) {
        r31 = -1;
    } else {
        obj3 = fn_801D16F0(ptr);
        count = 0;
        i = 0;
        while (i < total2) {
            r31 = fn_8017B4BC(obj3, i);
            if (fn_8017B5A4() == 9) {
                if ((u32)count >= r25) break;
                count++;
            }
            i++;
        }
    }
    if (r31 == -1) {
        return 0;
    }
    if (fn_801D16F0(*(u32*)(subctx + 4)) == 0) {
        return 0;
    }
    if (fn_8017B07C(/* r3 */ 0, r31) == 0) {
        return 0;
    }
    r23 = fn_800F92D4(r31);
    if (r23 == 0) {
        return 0;
    }
    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D85D4(0, r23);
    fn_800D6A00(7);
    fn_800D67BC(2);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, -1);
    fn_800D59B8(0, lbl_8047BE48, lbl_8047BE48);
    fn_800D61E4(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
    fn_800D5BA0(0, -1);
    fn_800D59B8(0, lbl_8047BE4C, lbl_8047BE4C);
    fn_800D6728();
    return 0;
}
#endif

/* fn_8004E790 - 0x8004E790 | size: 0x10c */
#if 0
asm void fn_8004E790(void) {
#include "src/game/scene_init_fn_8004E790.inc"
}
#else
u32 fn_8004E790(u8* ctx) {
    s32 curIdx;
    u8* subctx;
    u32 obj;
    s32 count;
    s32 i;
    u8* input;
    s32 total;
    u16 buttons;

    subctx = *(u8**)(ctx + 0x60);
    input = (u8*)fn_80105624();
    count = *(u32*)(subctx + 4);
    curIdx = *(s32*)(*(u32*)(subctx + 8));
    if (fn_8017B2CC(fn_801D16F0(count)) != 1) {
        obj = fn_801D16F0(count);
        total = fn_8017B448();
        count = 0;
        i = 0;
        while (i < total) {
            fn_8017B4BC(obj, i);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            i++;
        }
    } else {
        count = -1;
    }
    if (count < 2) {
        return 0;
    }
    buttons = *(u16*)(input + 6);
    if (buttons & 0x8) {
        curIdx++;
        if (curIdx >= count) {
            curIdx = 0;
        }
    }
    if (buttons & 0x4) {
        curIdx--;
        if (curIdx < 0) {
            curIdx = count - 1;
        }
    }
    *(u32*)(*(u32*)(subctx + 8)) = curIdx;
    return 0;
}
#endif

/* fn_8004E89C - 0x8004E89C | size: 0x44 */
#if 0
asm void fn_8004E89C(void) {
#include "src/game/scene_init_fn_8004E89C.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
void fn_8004E89C(u8* p) {
    u8* ctx;

    ctx = fn_80105624();
    if ((*(u16*)ctx & 0x10) == 0) {
        fn_80102ED4(p);
    }
}
#pragma pop
#endif

/* fn_8004E8E0 - 0x8004E8E0 | size: 0xe0 */
extern f32 lbl_8047BE50;
extern f32 lbl_8047BE4C;
#if 0
asm void fn_8004E8E0(void) {
#include "src/game/scene_init_fn_8004E8E0.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_8004E8E0(u8* p) {
    u8* r4;
    r4 = *(u8**)(p + 0x60);
    switch (*(s8*)(p + 1)) {
    case 0:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x77, 0x86);
            p[2] = 1;
        }
        break;
    case 2:
        {
            f32 f0;
            f32 f1 = lbl_8047BE4C;
            f32* fp = (f32*)(*(u32*)r4);
            f0 = *fp + lbl_8047BE50;
            *fp = f0;
            if (f0 >= f1) {
                *fp = *fp - f1;
            }
        }
        break;
    case 3:
        if (*(s8*)(p + 2) == 0) {
            fn_801080CC(0x77, 0x8a);
            p[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
#endif

/* fn_8004E9C0 - 0x8004E9C0 | size: 0x11c */
extern u32 lbl_804788E8;
extern u32 lbl_8047A534;
extern u32 lbl_8047A530;
extern u32 lbl_8047A538;
extern f32 lbl_8047A53C;
extern f32 lbl_8047BE48;
/* Init sound scene: read ROM offsets, start BGM, run scene 0x77, wait for BGM. */
#if 0
asm void fn_8004E9C0(s32 a) {
#include "src/game/scene_init_fn_8004E9C0.inc"
}
#else
#pragma peephole off
void fn_8004E9C0(s32 a) {
    u32 snd;
    u32 buf[6];

    if (lbl_804788E8 != 0) {
        lbl_804788E8 = 0;
        lbl_8047A534 = (u32)*(s16*)(lbl_802EF0A8 + 0x18cc6);
        lbl_8047A530 = (u32)*(s16*)(lbl_802EF0A8 + 0x18caa);
    }
    lbl_8047A538 = 0;
    snd = fn_801D16F0(a);
    if (snd != 0) {
        fn_8017B3E4(snd);
        lbl_8047A53C = lbl_8047BE48;
        buf[0] = (u32)&lbl_8047A53C;
        buf[1] = (u32)a;
        buf[2] = (u32)&lbl_8047A538;
        buf[3] = lbl_8047A534;
        buf[4] = lbl_8047A530;
        fn_801026A4(0x77, (u32)fn_801046B8(), 0, 0, 1, 1, (u32)buf);
        while (fn_8017B2CC(snd) == 1) {
            _threadSwitch();
        }
        fn_80102510(0x77);
        menuCloseSync(0x77, 1);
        fn_8017B1CC(snd);
        fn_800F915C(snd);
    }
}
#pragma peephole on
#endif

/* fn_8004EC54 - 0x8004EC54 | size: 0x178 */
extern u8* fn_801FBFBC();
extern u8 lbl_803A6AB0[];
#if 0
asm void fn_8004EC54(void) {
#include "src/game/scene_init_fn_8004EC54.inc"
}
#else
s32 fn_8004EC54(void) {
    u16 i;
    u8* src;
    u8* dst;
    s32 result;

    i = 0;
    while ((u16)i < 0x229) {
        src = fn_801FBFBC(i);
        dst = lbl_803A6AB0 + (u16)i * 0x14;
        ((u32*)dst)[0] = ((u32*)src)[0];
        ((u32*)dst)[1] = ((u32*)src)[1];
        ((u32*)dst)[2] = ((u32*)src)[2];
        ((u32*)dst)[3] = ((u32*)src)[3];
        ((u32*)dst)[4] = ((u32*)src)[4];
        i++;
    }
    for (;;) {
        result = fn_801026A4(0x88, 0, 0, 0, 1, 0);
        if (result == -1) {
            fn_80102568(0x88, 0, 1);
            i = 0;
            while ((u16)i < 0x229) {
                src = fn_801FBFBC(i);
                dst = lbl_803A6AB0 + (u16)i * 0x14;
                ((u32*)src)[0] = ((u32*)dst)[0];
                ((u32*)src)[1] = ((u32*)dst)[1];
                ((u32*)src)[2] = ((u32*)dst)[2];
                ((u32*)src)[3] = ((u32*)dst)[3];
                ((u32*)src)[4] = ((u32*)dst)[4];
                i++;
            }
            return -1;
        } else if (result == -2) {
            if (fn_8010264C(0x44, 1) == 0) {
                fn_80102568(0x44, 0, 1);
                fn_80102568(0x88, 0, 1);
                return 1;
            }
            fn_80102568(0x44, 0, 1);
        }
    }
}
#endif

/* fn_8004EDCC - 0x8004EDCC | size: 0xa94 */
extern u32 fn_801FB1C0(u32 a, u32 b, u32 c, u32 d);
extern u8 fn_8001E224(u32 ctx, s32* out, u32 a, u32 b, u32 c, u32 d);
extern void fn_801FAA58(u32 a, u32 b, u32 c, u32 d, s32 e);
extern void fn_8001E200(void);
#if 1
asm void fn_8004EDCC(void) {
#include "src/game/scene_init_fn_8004EDCC.inc"
}
#else
void fn_8004EDCC(void) { /* TODO */ }
#endif

/* fn_8004F860 - 0x8004F860 | size: 0x5dc */
#if 1
asm void fn_8004F860(void) {
#include "src/game/scene_init_fn_8004F860.inc"
}
#else
void fn_8004F860(void) { /* TODO */ }
#endif

/* fn_8004FE3C - 0x8004FE3C | size: 0xa08 */
#if 1
asm void fn_8004FE3C(void) {
#include "src/game/scene_init_fn_8004FE3C.inc"
}
#else
void fn_8004FE3C(void) { /* TODO */ }
#endif

/* fn_80051710 - 0x80051710 | size: 0x728 */
extern void fn_801FCAD0(void);
#if 1
asm void fn_80051710(void) {
#include "src/game/scene_init_fn_80051710.inc"
}
#else
void fn_80051710(void) { /* TODO */ }
#endif

/* fn_80051E38 - 0x80051E38 | size: 0x122c */
extern void fn_801FC658(void);
extern void fn_80008518(void);
extern void fn_80008460(void);
#if 1
asm void fn_80051E38(void) {
#include "src/game/scene_init_fn_80051E38.inc"
}
#else
void fn_80051E38(void) { /* TODO */ }
#endif

/* fn_80053064 - 0x80053064 | size: 0xac */
#if 0
asm void fn_80053064(void) {
#include "src/game/scene_init_fn_80053064.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma optimization_level 4
void fn_80053064(u32 a, u32 b, u32 c, s32 max, s32 min) {
    s32 val;
    u8 result;
    result = (u8)fn_8001E224(fn_801FB1C0(0, a, b, c), &val, 0, 0x32, 0x32, 0);
    if (result == 1) {
        if (val > max) {
            val = max;
        }
        if (val < min) {
            val = min;
        }
        fn_801FAA58(0, a, b, c, val);
    }
    fn_8001E200();
}
#pragma pop
#endif

/* fn_8005344C - 0x8005344C | size: 0xd0 */
#if 0
asm void fn_8005344C(void) {
#include "src/game/scene_init_fn_8005344C.inc"
}
#else
#pragma optimization_level 4
u32 fn_8005344C(u8* ctx, u8* p) {
    u8 r31;
    u8* obj;
    s16 coord[2];
    u32 out;
    r31 = 0;
    if (fn_80057A08() != 0) {
        obj = (u8*)fn_80104704(0x94);
        if (obj != NULL) {
            if (fn_80058F08(&out, (s8)obj[0x95]) == 0) {
                s16 field6;
                r31 = 1;
                fn_80057094(&coord[0], &coord[1]);
                field6 = *(s16*)(p + 6);
                *(s16*)(p + 0x50) = (s16)(coord[0] + *(s16*)(lbl_802EF0A8 + (u32)field6 * 0x1c + 2));
                *(s16*)(p + 0x52) = (s16)(coord[1] + *(s16*)(lbl_802EF0A8 + (u32)field6 * 0x1c + 4));
            }
        }
    }
    fn_80109220((u32)p, (u32)r31);
    return 0;
}
#endif

/* fn_8005351C - 0x8005351C | size: 0x20c */
extern u8 lbl_80267338[];
extern f64 lbl_8047BE58;
extern f32 lbl_8047A540;
#if 0
asm void fn_8005351C(void) {
#include "src/game/scene_init_fn_8005351C.inc"
}
#else
#pragma peephole off
u32 fn_8005351C(u32 unused, u8* p) {
    s16 tbl[12];
    u8 show;
    f32 t;

    ((u32*)tbl)[0] = ((u32*)lbl_80267338)[0];
    ((u32*)tbl)[1] = ((u32*)lbl_80267338)[1];
    ((u32*)tbl)[2] = ((u32*)lbl_80267338)[2];
    ((u32*)tbl)[3] = ((u32*)lbl_80267338)[3];
    ((u32*)tbl)[4] = ((u32*)lbl_80267338)[4];
    ((u32*)tbl)[5] = ((u32*)lbl_80267338)[5];

    if (fn_80057E40() != 2) {
        show = 1;
    } else {
        show = 0;
    }
    fn_80109220((u32)p, show);
    if (show != 0) {
        t = lbl_8047A540;
        *(s16*)(p + 0x50) = (s16)(s32)((f32)(s32)(tbl[6] - tbl[0]) * t + (f32)(s32)tbl[0]);
        *(s16*)(p + 0x52) = (s16)(s32)((f32)(s32)(tbl[7] - tbl[1]) * t + (f32)(s32)tbl[1]);
        *(s16*)(p + 0x54) = (s16)(s32)((f32)(s32)(tbl[8] - tbl[2]) * t + (f32)(s32)tbl[2]);
        *(s16*)(p + 0x56) = (s16)(s32)((f32)(s32)(tbl[9] - tbl[3]) * t + (f32)(s32)tbl[3]);
        p[0x67] = (u8)(s32)((f32)(s32)(*(u32*)&tbl[10] - *(u32*)&tbl[4]) * t + (f32)(s32)*(u32*)&tbl[4]);
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_80053728 - 0x80053728 | size: 0x50 */
#if 0
asm void fn_80053728(void) {
#include "src/game/scene_init_fn_80053728.inc"
}
#else
#pragma optimization_level 4
u32 fn_80053728(u8* p, u32 q) {
    u8 val;

    if (fn_80057E40() == 2) {
        val = 0;
    } else {
        val = 1;
    }
    fn_80109220(q, val);
    return 0;
}
#endif

/* fn_80053A60 - 0x80053A60 | size: 0x68 */
extern u32 fn_80057270(void);
extern u8 fn_8011E820(void);
#if 0
asm void fn_80053A60(void) {
#include "src/game/scene_init_fn_80053A60.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053A60(u8* p, u32 q) {
    u8 val;

    val = 0;
    if (fn_80057270() != 0) {
        if (fn_8011E820() & 0x08) {
            val = 1;
        }
    }
    fn_80109220(q, val);
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053AC8 - 0x80053AC8 | size: 0x68 */
#if 0
asm void fn_80053AC8(void) {
#include "src/game/scene_init_fn_80053AC8.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053AC8(u8* p, u32 q) {
    u8 val;

    val = 0;
    if (fn_80057270() != 0) {
        if (fn_8011E820() & 0x04) {
            val = 1;
        }
    }
    fn_80109220(q, val);
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053B30 - 0x80053B30 | size: 0x68 */
#if 0
asm void fn_80053B30(void) {
#include "src/game/scene_init_fn_80053B30.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053B30(u8* p, u32 q) {
    u8 val;

    val = 0;
    if (fn_80057270() != 0) {
        if (fn_8011E820() & 0x02) {
            val = 1;
        }
    }
    fn_80109220(q, val);
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053B98 - 0x80053B98 | size: 0x68 */
#if 0
asm void fn_80053B98(void) {
#include "src/game/scene_init_fn_80053B98.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053B98(u8* p, u32 q) {
    u8 val;

    val = 0;
    if (fn_80057270() != 0) {
        if (fn_8011E820() & 0x01) {
            val = 1;
        }
    }
    fn_80109220(q, val);
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053C00 - 0x80053C00 | size: 0x84 */
extern u16 fn_801230E0(void);
#if 0
asm void fn_80053C00(void) {
#include "src/game/scene_init_fn_80053C00.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053C00(u8* p, u8* q) {
    u32 v;
    if (fn_80057270() != 0) {
        v = fn_801230E0();
        if (v != 0) {
            if (fn_801440A0(v) != 0) {
                fn_80144088();
                fn_80132A38(0x37, fn_800FA280());
                fn_800FB680(0, 0, -1, 0xe7);
            }
        }
    }
    *(u32*)(q + 0x4c) = 0;
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053C84 - 0x80053C84 | size: 0x64 */
#if 0
asm void fn_80053C84(void) {
#include "src/game/scene_init_fn_80053C84.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053C84(u8* p, u32 q) {
    u8 val;

    val = 0;
    if (fn_80057270() != 0) {
        if (fn_801230E0() != 0) {
            val = 1;
        }
    }
    fn_80109220(q, val);
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053CE8 - 0x80053CE8 | size: 0x7c */
extern u8 fn_8001DA60(void);
#if 0
asm void fn_80053CE8(void) {
#include "src/game/scene_init_fn_80053CE8.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
u32 fn_80053CE8(u8* p, u8* q) {
    u32 val;

    val = 0;
    if (fn_80057270() != 0) {
        switch (fn_8001DA60()) {
        case 0:
            val = 0xd67;
            break;
        case 1:
            val = 0xd68;
            break;
        case 2:
            break;
        }
    }
    *(u32*)(q + 0x4c) = val;
    return 0;
}
#pragma pop
#endif

/* fn_80053D64 - 0x80053D64 | size: 0x70 */
#if 0
asm void fn_80053D64(void) {
#include "src/game/scene_init_fn_80053D64.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
#pragma peephole off
u32 fn_80053D64(u8* p, u8* q) {
    if (fn_80057270() != 0) {
        fn_8011F5C8();
        fn_8011E778();
        fn_8011E760();
        fn_80132A38(0x37, fn_800FA280());
        fn_800FB680(0, 0, -1, 0xe7);
    }
    *(u32*)(q + 0x4c) = 0;
    return 0;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80053DD4 - 0x80053DD4 | size: 0xa8 */
extern u8 fn_8011F4A8(void);
#if 0
asm void fn_80053DD4(void) {
#include "src/game/scene_init_fn_80053DD4.inc"
}
#else
#pragma optimization_level 4
u32 fn_80053DD4(u8* p, u8* q) {
    u8 pct;
    s32 rows;

    rows = 0;
    if (fn_80057270() != 0) {
        pct = fn_8011F4A8();
        if (pct >= 0x64) {
            rows = 3;
        } else {
            rows = 2;
        }
        fn_800FB680(*(s16*)(q + 0x54) - rows * 0xf - (s32)(fn_800FA444(0x1b82) >> 16), 0, -1, 0x1b82);
        fn_80132A38(0x34, pct);
        rows = 0xde;
    }
    *(u32*)(q + 0x4c) = rows;
    return 0;
}
#endif

/* fn_80053E7C - 0x80053E7C | size: 0x5c */
extern u32 fn_8011F4F0(void);
#if 0
asm void fn_80053E7C(void) {
#include "src/game/scene_init_fn_80053E7C.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_80053E7C(u8* p) {
    u32 v;
    if (fn_80057270() == 0) {
        return 0;
    }
    v = fn_8011F4F0();
    fn_80132A38(0x37, v);
    fn_800FB680(0, 0, -1, 0xe7);
    return 0;
}
#pragma scheduling off
#endif

/* fn_80053ED8 - 0x80053ED8 | size: 0x548 */
extern u8 fn_80123FBC(void* a);
extern void fn_800DC0D4(void);
extern void fn_800DC14C(void);
extern void fn_800DBFD4(void);
extern void fn_800DC04C(void);
extern void fn_800EF4FC(void);
extern void fn_800EF4F4(void);
extern void fn_8005D858(void);
extern void fn_80104160(void);
extern u8 lbl_802E61E8[];
extern f32 lbl_8047BE80;
extern f32 lbl_8047A558;
extern u8 lbl_80267320[];
extern f32 lbl_8047A550;
extern u32 lbl_8047BE6C;
extern u32 lbl_8047BE64;
extern f32 lbl_8047BE60;
extern u32 lbl_8047BE84;
extern f32 lbl_8047A54C;
extern u32 lbl_8047A548;
extern u32 lbl_8047BE78;
extern f32 lbl_8047BE68;
#if 1
asm void fn_80053ED8(void) {
#include "src/game/scene_init_fn_80053ED8.inc"
}
#else
void fn_80053ED8(void) { /* TODO */ }
#endif

/* fn_80054420 - 0x80054420 | size: 0x88 */
extern f32 lbl_8047BE80;
extern f32 lbl_8047A558;
extern u8 lbl_802E61E8[];
#if 0
asm void fn_80054420(void) {
#include "src/game/scene_init_fn_80054420.inc"
}
#else
u32 fn_80054420(u32 unused, u8* entry) {
    s32 i;
    s16 val;
    u8* tbl;

    tbl = lbl_802E61E8;
    val = *(s16*)(entry + 6);
    for (i = 0; i < 0x11; i++) {
        if (val == *(s32*)tbl) {
            break;
        }
        tbl += 8;
    }
    if (i >= 0x11) {
        return 0;
    }
    *(s16*)(entry + 0x52) = (s16)((s32)(lbl_8047BE80 * lbl_8047A558) + *(s16*)(tbl + 4));
    return 0;
}
#endif

/* fn_800544A8 - 0x800544A8 | size: 0x1a4 */
extern u32 lbl_8047A544;
extern f32 lbl_8047BE68;
extern f32 lbl_8047BE60;
extern f32 lbl_8047A550;
extern f32 lbl_8047A54C;
extern f32 lbl_8047A540;
extern f32 lbl_8047A554;
extern f32 lbl_8047A558;
extern f32 lbl_8047BE88;
#if 0
asm void fn_800544A8(void) {
#include "src/game/scene_init_fn_800544A8.inc"
}
#else
s32 fn_800544A8(u8* ctx) {
    switch ((s8)ctx[1]) {
    case 0:
        if ((s8)ctx[2] != 0) { break; }
        lbl_8047A550 = lbl_8047BE68;
        lbl_8047A54C = lbl_8047BE60;
        lbl_8047A540 = lbl_8047BE68;
        if (lbl_8047A544 != 0) {
            fn_801080CC(0x8f, 0xff);
        }
        ctx[2] = 1;
        break;
    case 2:
        if (lbl_8047A554 > lbl_8047BE68) {
            lbl_8047A558 = lbl_8047A558 + lbl_8047A554;
            if (lbl_8047A558 >= lbl_8047BE60) {
                lbl_8047A558 = lbl_8047BE60;
                lbl_8047A554 = lbl_8047BE68;
            }
        }
        if (lbl_8047A554 < lbl_8047BE68) {
            lbl_8047A558 = lbl_8047A558 + lbl_8047A554;
            if (lbl_8047A558 <= lbl_8047BE68) {
                lbl_8047A558 = lbl_8047BE68;
                lbl_8047A554 = lbl_8047BE68;
            }
        }
        lbl_8047A550 = lbl_8047A550 + lbl_8047BE88;
        if (lbl_8047A550 >= lbl_8047BE60) {
            lbl_8047A550 = lbl_8047A550 - lbl_8047BE60;
        }
        if (lbl_8047A54C < lbl_8047BE60) {
            lbl_8047A54C = lbl_8047A54C + lbl_8047BE88;
            if (lbl_8047A54C > lbl_8047BE60) {
                lbl_8047A54C = lbl_8047BE60;
            }
        }
        lbl_8047A540 = lbl_8047A540 + lbl_8047BE88;
        if (lbl_8047A540 >= lbl_8047BE60) {
            lbl_8047A540 = lbl_8047A540 - lbl_8047BE60;
        }
        break;
    case 3:
        if ((s8)ctx[2] != 0) { break; }
        fn_801080CC(0x8f, 0x103);
        ctx[2] = 1;
        break;
    }
    return 0;
}
#endif

/* fn_8005464C - 0x8005464C | size: 0x24 */
extern f32 lbl_8047A54C;
extern f32 lbl_8047BE8C;
#if 0
asm void fn_8005464C(void) {
#include "src/game/scene_init_fn_8005464C.inc"
}
#else
#pragma optimization_level 4
u32 fn_8005464C(void) {
    return !(lbl_8047A54C >= lbl_8047BE8C);
}
#endif

/* fn_80054670 - 0x80054670 | size: 0x10 */
extern f32 lbl_8047BE68;
extern u32 lbl_8047A548;
extern f32 lbl_8047A54C;
#if 0
asm void fn_80054670(void) {
#include "src/game/scene_init_fn_80054670.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma optimization_level 4
void fn_80054670(u32 a) {
    lbl_8047A548 = a;
    lbl_8047A54C = lbl_8047BE68;
}
#pragma pop
#endif

/* fn_80054680 - 0x80054680 | size: 0x40 */
extern f32 lbl_8047A554;
extern f32 lbl_8047BE68;
extern f32 lbl_8047A558;
extern f32 lbl_8047BE60;
#if 0
asm void fn_80054680(void) {
#include "src/game/scene_init_fn_80054680.inc"
}
#else
#pragma optimization_level 4
u32 fn_80054680(void) {
    if (lbl_8047A554 > lbl_8047BE68) { return 3; }
    if (lbl_8047A554 < lbl_8047BE68) { return 2; }
    return lbl_8047A558 >= lbl_8047BE60;
}
#endif

/* fn_800546C0 - 0x800546C0 | size: 0x30 */
extern f32 lbl_8047BE60;
extern f32 lbl_8047BE68;
extern f32 lbl_8047A558;
extern f32 lbl_8047A554;
extern u32 lbl_8047BE90;
#if 0
asm void fn_800546C0(void) {
#include "src/game/scene_init_fn_800546C0.inc"
}
#else
#pragma optimization_level 4
void fn_800546C0(u32 flag) {
    if (flag != 0) { lbl_8047A558 = lbl_8047BE60; lbl_8047A554 = lbl_8047BE68; }
    else { lbl_8047A558 = lbl_8047BE68; lbl_8047A554 = lbl_8047BE90; }
}
#endif

/* fn_800546F0 - 0x800546F0 | size: 0x2c */
extern f32 lbl_8047BE68;
extern f32 lbl_8047A558;
extern f32 lbl_8047A554;
extern f32 lbl_8047BE60;
extern u32 lbl_8047BE94;
#if 0
asm void fn_800546F0(void) {
#include "src/game/scene_init_fn_800546F0.inc"
}
#else
#pragma optimization_level 4
void fn_800546F0(u32 flag) {
    if (flag != 0) { lbl_8047A558 = lbl_8047BE68; lbl_8047A554 = lbl_8047BE68; }
    else { lbl_8047A558 = lbl_8047BE60; lbl_8047A554 = lbl_8047BE94; }
}
#endif

/* fn_8005471C - 0x8005471C | size: 0x44 */
extern u8 lbl_803A9720[];
#if 0
asm void fn_8005471C(void) {
#include "src/game/scene_init_fn_8005471C.inc"
}
#else
#pragma optimization_level 4
void fn_8005471C(void) {
    fn_80102568(0x8f, 2, 0);
    menuCloseSync(0x8f, 1);
    fn_8010A420(lbl_803A9720);
}
#endif

/* fn_80054760 - 0x80054760 | size: 0x1b4 */
extern void fn_8005D934(void);
extern void fn_80124A60(u8* a);
extern u32 lbl_804788F8;
extern u32 lbl_8047A544;
extern f32 lbl_8047BE60;
extern f32 lbl_8047BE68;
extern f32 lbl_8047A558;
extern f32 lbl_8047A554;
extern u8 lbl_803A95E8[];
#if 1
asm void fn_80054760(s32 a, s32 b) {
#include "src/game/scene_init_fn_80054760.inc"
}
#else
void fn_80054760(s32 a, s32 b) { /* TODO */ }
#endif

/* fn_800549F0 - 0x800549F0 | size: 0x3c */
#if 0
asm void fn_800549F0(void) {
#include "src/game/scene_init_fn_800549F0.inc"
}
#else
#pragma optimization_level 4
u32 fn_800549F0(u8* p, u32 q) {
    if (*(s32*)(*(u32*)(p + 0x60) + 4) != 2) {
        fn_80109220(q, 0);
    }
    return 0;
}
#endif

/* fn_80054A2C - 0x80054A2C | size: 0x3c */
#if 0
asm void fn_80054A2C(void) {
#include "src/game/scene_init_fn_80054A2C.inc"
}
#else
#pragma optimization_level 4
u32 fn_80054A2C(u8* p, u32 q) {
    if (*(s32*)(*(u32*)(p + 0x60) + 4) != 1) {
        fn_80109220(q, 0);
    }
    return 0;
}
#endif

/* fn_80054A68 - 0x80054A68 | size: 0x3c */
#if 0
asm void fn_80054A68(void) {
#include "src/game/scene_init_fn_80054A68.inc"
}
#else
#pragma optimization_level 4
u32 fn_80054A68(u8* p, u32 q) {
    if (*(s32*)(*(u32*)(p + 0x60) + 4) != 0) {
        fn_80109220(q, 0);
    }
    return 0;
}
#endif

/* fn_80054AA4 - 0x80054AA4 | size: 0x3c */
#if 0
asm void fn_80054AA4(void) {
#include "src/game/scene_init_fn_80054AA4.inc"
}
#else
#pragma optimization_level 4
u32 fn_80054AA4(u8* p, u32 q) {
    if (*(s32*)(*(u32*)(p + 0x60)) == 0) {
        fn_80109220(q, 0);
    }
    return 0;
}
#endif

/* fn_80054AE0 - 0x80054AE0 | size: 0x3c */
#if 0
asm void fn_80054AE0(void) {
#include "src/game/scene_init_fn_80054AE0.inc"
}
#else
#pragma optimization_level 4
u32 fn_80054AE0(u8* p, u32 q) {
    if (*(s32*)(*(u32*)(p + 0x60)) != 0) {
        fn_80109220(q, 0);
    }
    return 0;
}
#endif

/* fn_80054B1C - 0x80054B1C | size: 0x128 */
#if 0
asm void fn_80054B1C(void) {
#include "src/game/scene_init_fn_80054B1C.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
u32 fn_80054B1C(u32 a, s32 b) {
    s32 result;
    u32 buf[2];

    buf[0] = (u32)b;
    buf[1] = a;
    result = fn_801026A4(0x9c, (u32)fn_801046B8(), 0, 0, 1, 1, (u32)buf);
    fn_80102510(0x9c);
    menuCloseSync(0x9c, 1);
    switch (result) {
    case 0:
        switch (a) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        default: return 0;
        }
    case 1: return 3;
    case 2:
        if (b != 0) { return 5; }
        return 4;
    case 3: return 6;
    case 4: return 7;
    default: return 8;
    }
}
#pragma pop
#endif

/* fn_80054C44 - 0x80054C44 | size: 0x238 */
extern void fn_80102F38();
extern u8* fn_80104318();
extern void fn_80057830(s32 a, s32 b, s32 c);
extern u8 lbl_80267398[];
#if 0
asm void fn_80054C44(void) {
#include "src/game/scene_init_fn_80054C44.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
u32 fn_80054C44(u8* ctx) {
    u8* input;
    u16 buttons;
    s32 type;
    s32 type2;
    u8* ptr;

    input = (u8*)fn_80105624();
    if ((s32)fn_800573C0() != 0) { return 0; }
    if ((s32)fn_800566E8() != 0) { return 0; }
    if (*(u16*)(input + 6) & 0xC0) {
        if ((s32)fn_80057694() != 0) {
            fn_800576A4(0);
        } else {
            fn_800576A4(1);
        }
    }
    buttons = *(u16*)input;
    if (buttons & 0x400) {
        fn_800567AC();
        return 0;
    }
    if (buttons & 0x200) {
        fn_80056704();
        return 0;
    }
    {
        s8 slot = (s8)ctx[0x95];
        if (slot < 0 || slot >= 0x20) {
            type = 3;
        } else {
            type = *(u32*)(lbl_80267398 + slot * 0xc + 4);
        }
    }
    if (type == 1) {
        if (buttons == 0x8) {
            fn_800567AC();
            return 0;
        }
        if (buttons == 0x4) {
            fn_80056704();
            return 0;
        }
    }
    if (type == 2) {
        if (*(u16*)(input + 6) & 0x5) {
            return 0;
        }
        if (*(u16*)(input + 6) & 0xa) {
            ctx[0x95] = 0x1e;
        }
    }
    if ((s8)ctx[0x95] == (s8)ctx[0x97]) {
        fn_80102F38(ctx);
        {
            s8 s97 = (s8)ctx[0x97];
            if (s97 < 0 || s97 >= 0x20) {
                type = 3;
            } else {
                type = *(u32*)(lbl_80267398 + s97 * 0xc + 4);
            }
        }
        {
            s8 s95 = (s8)ctx[0x95];
            if (s95 < 0 || s95 >= 0x20) {
                type2 = 3;
            } else {
                type2 = *(u32*)(lbl_80267398 + s95 * 0xc + 4);
            }
        }
        if (type == 0 && type2 == 2) {
            ctx[0x95] = 0x1e;
        }
    }
    if ((s8)ctx[0x95] != (s8)ctx[0x97]) {
        ptr = fn_80104318(ctx);
        fn_80057830(*(s16*)(ptr + 2), *(s16*)(ptr + 4), 0);
        return 0;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80054E7C - 0x80054E7C | size: 0x4c */
#if 0
asm void fn_80054E7C(void) {
#include "src/game/scene_init_fn_80054E7C.inc"
}
#else
#pragma scheduling on
void fn_80054E7C(u8* ctx) {
    fn_80105624();
    if ((s32)fn_800573C0() == 0) {
        if ((s32)fn_800566E8() == 0) {
            fn_80102ED4(ctx);
        }
    }
}
#pragma scheduling off
#endif

/* fn_80054EC8 - 0x80054EC8 | size: 0x1ec */
extern s32 fn_80056A78(void);
extern void* fn_80134EF0(u32 a, s32 b, s32 c);
extern void fn_80134AF8(u32 a, s32 b, s32 c);
extern void fn_80134E10(u32 a, void* b, s32 c, s32 d);
extern u32 lbl_8047A560;
#if 0
asm void fn_80054EC8(u8* ctx) {
#include "src/game/scene_init_fn_80054EC8.inc"
}
#else
#pragma peephole off
void fn_80054EC8(u8* ctx) {
    s32 tbl_val;
    s32 state;
    s32 slot;

    state = (s32)fn_800576B4();
    switch (state) {
    case 1:
        if ((s32)fn_80057538() == 0) { break; }
        slot = (s8)ctx[0x95];
        if (slot >= 0 && slot < 0x20) {
            tbl_val = *(s32*)(lbl_80267398 + (u32)slot * 0xc + 8);
        }
        {
            s32 snd = fn_80056A78();
            fn_80134EF0(0, (s8)snd, (s8)tbl_val);
            fn_800574FC(0);
            fn_80134AF8(0, (s8)snd, (s8)tbl_val);
        }
        fn_800576C4(2);
        break;
    case 2:
        if ((s32)fn_80057538() == 0) { break; }
        fn_800576C4(3);
        if ((s32)lbl_8047A560 != 0) {
            ctx[0x98] = 1;
        }
        break;
    case 4:
        if ((s32)fn_80057538() == 0) { break; }
        slot = (s8)ctx[0x95];
        if (slot >= 0 && slot < 0x20) {
            tbl_val = *(s32*)(lbl_80267398 + (u32)slot * 0xc + 8);
        }
        {
            s32 snd = fn_80056A78();
            fn_80134E10(0, fn_800574E0(), (s8)snd, (s8)tbl_val);
            fn_800574A8();
        }
        fn_800576C4(5);
        break;
    case 5:
        if ((s32)fn_80057538() == 0) { break; }
        fn_800576C4(0);
        break;
    case 6:
        if ((s32)fn_80057538() == 0) { break; }
        slot = (s8)ctx[0x95];
        if (slot >= 0 && slot < 0x20) {
            tbl_val = *(s32*)(lbl_80267398 + (u32)slot * 0xc + 8);
        }
        {
            s32 snd = fn_80056A78();
            fn_80134E10(0, fn_800574E0(), (s8)snd, (s8)tbl_val);
            fn_800574A8();
        }
        fn_80057400();
        fn_800576C4(3);
        break;
    }
}
#pragma peephole on
#endif

/* fn_800550B4 - 0x800550B4 | size: 0xe0 */
#if 0
asm void fn_800550B4(void) {
#include "src/game/scene_init_fn_800550B4.inc"
}
#else
#pragma peephole off
#pragma optimization_level 2
#pragma scheduling on
s32 fn_800550B4(u8* ctx) {
    s32 flag;
    u8* ptr;

    switch ((s8)ctx[1]) {
    case 0:
        if ((s8)ctx[2] == 0) {
            flag = 1;
        } else {
            flag = 0;
        }
        ptr = fn_80104318(ctx);
        fn_80057830(*(s16*)(ptr + 2), *(s16*)(ptr + 4), flag);
        ctx[2] = 1;
        break;
    case 2:
        fn_80057948();
        fn_80056854();
        if ((s32)fn_801046B8() == *(s32*)(ctx + 4)) {
            fn_80054EC8(ctx);
        }
        break;
    case 3:
        if ((s8)ctx[2] == 0) {
            ctx[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma scheduling off
#endif

/* fn_80055194 - 0x80055194 | size: 0x38 */
#if 0
asm void fn_80055194(void) {
#include "src/game/scene_init_fn_80055194.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
u32 fn_80055194(u32* out, s32 index) {
    u8* entry;
    if (index < 0 || index >= 0x20) {
        return 3;
    }
    entry = lbl_80267398 + (index * 0xc);
    *out = *(u32*)(entry + 8);
    return *(u32*)(entry + 4);
}
#pragma peephole on
#endif

/* fn_800551CC - 0x800551CC | size: 0x108 */
extern u32 fn_801347D8(void);
#if 0
asm void fn_800551CC(void) {
#include "src/game/scene_init_fn_800551CC.inc"
}
#else
void* fn_800551CC(void* dflt, s32 dir, u32* state) {
    s32 i;
    void* found;
    s8 slot;

    slot = (s8)state[1];
    if (*(s32*)state != 0) {
        return dflt;
    }
    if (dir == 1) {
        i = (s32)state[2] - 1;
        while (i >= 0) {
            found = fn_80134EF0(0, (s8)slot, (s8)i);
            if ((u8)fn_80123FBC(found) != 0) {
                break;
            }
            i--;
        }
        if (i >= 0) {
            state[2] = (u32)i;
            return found;
        }
    }
    if (dir == 2) {
        i = (s32)state[2] + 1;
        while (i < (s8)fn_801347D8()) {
            found = fn_80134EF0(0, (s8)slot, (s8)i);
            if ((u8)fn_80123FBC(found) != 0) {
                break;
            }
            i++;
        }
        if (i < (s8)fn_801347D8()) {
            state[2] = (u32)i;
            return found;
        }
    }
    return dflt;
}
#endif

/* fn_800558B8 - 0x800558B8 | size: 0x2e0 */
extern void fn_8005D3D0(void);
extern void fn_800552D4(void);
extern void fn_8005D26C(void);
extern void fn_80057B34(void);
extern u32 lbl_8047A560;
extern u32 lbl_8047BE98;
#if 1
asm void fn_800558B8(void) {
#include "src/game/scene_init_fn_800558B8.inc"
}
#else
void fn_800558B8(void) { /* TODO */ }
#endif

/* fn_80055B98 - 0x80055B98 | size: 0x94 */
#if 0
asm void fn_80055B98(void) {
#include "src/game/scene_init_fn_80055B98.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
#pragma peephole off
void fn_80055B98(s32 idx) {
    fn_801026A4(0x10e, 0x1f, 0, 0, 0, 0);
    fn_80056B74(idx, 1);
    while ((s32)fn_80055E38(idx) == 0) {
        _threadSwitch();
    }
    fn_80054760(0, 1);
    fn_80057A64(0, 0);
    fn_800558B8();
    fn_80057A38();
    fn_8005471C();
    fn_80056A80();
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_80055C2C - 0x80055C2C | size: 0xa8 */
extern u32 fn_801347E0(void);
extern s32 fn_801348EC(s32 base, s32 idx);
#if 0
asm void fn_80055C2C(void) {
#include "src/game/scene_init_fn_80055C2C.inc"
}
#else
u32 fn_80055C2C(u8* ctx, u8* p) {
    s8 slot;
    s8 base;
    s8 idx;

    slot = (s8)ctx[0x95];
    if (slot < 0) {
        return 0;
    }
    if (slot >= (s8)fn_801347E0()) {
        return 0;
    }
    base = (s8)fn_801347D8();
    idx = (s8)fn_801348EC(0, slot);
    if (idx < 0) {
        return 0;
    }
    fn_80132A38(0x34, (u32)(s32)idx);
    fn_80132A38(0x35, (u32)(s32)base);
    *(u32*)(p + 0x4c) = 0x1b7f;
    return 0;
}
#endif

/* fn_80055CD4 - 0x80055CD4 | size: 0x60 */
extern void* fn_80134A98(void* base, s8 index);
#if 0
asm void fn_80055CD4(void) {
#include "src/game/scene_init_fn_80055CD4.inc"
}
#else
#pragma optimization_level 4
void fn_80055CD4(u8* ctx, u8* p) {
    void* r;
    r = fn_80134A98(NULL, (s8)ctx[0x95]);
    if (r == NULL) { return; }
    fn_80132A38(0x37, (u32)r);
    *(u32*)(p + 0x4c) = 0xcf;
}
#endif

/* fn_80055D34 - 0x80055D34 | size: 0xac */
#if 0
asm void fn_80055D34(void) {
#include "src/game/scene_init_fn_80055D34.inc"
}
#else
#pragma optimization_level 4
void fn_80055D34(u8* ctx) {
    u8* s;
    u16 flags;
    u32 max;
    s = fn_80105624();
    flags = *(u16*)(s + 6);
    if (flags & 0x8) {
        max = fn_801347E0();
        ctx[0x95] = (u8)(ctx[0x95] + 1);
        if ((s8)ctx[0x95] >= (s8)max) {
            ctx[0x95] = 0;
        }
    }
    flags = *(u16*)(s + 6);
    if (flags & 0x4) {
        ctx[0x95] = (u8)(ctx[0x95] - 1);
        if ((s8)ctx[0x95] < 0) {
            max = fn_801347E0();
            ctx[0x95] = (u8)(s8)(max - 1);
        }
    }
}
#endif

/* fn_80055DE0 - 0x80055DE0 | size: 0x30 */
#if 0
asm void fn_80055DE0(void) {
#include "src/game/scene_init_fn_80055DE0.inc"
}
#else
#pragma optimization_level 4
void fn_80055DE0(void) {
    fn_80102510(0xa3);
    menuCloseSync(0xa3, 1);
}
#endif

/* fn_80055E10 - 0x80055E10 | size: 0x28 */
#if 0
asm void fn_80055E10(void) {
#include "src/game/scene_init_fn_80055E10.inc"
}
#else
#pragma optimization_level 4
u32 fn_80055E10(void) {
    return fn_8010264C(0xa3, 1);
}
#endif

/* fn_80055E38 - 0x80055E38 | size: 0x80 */
extern u32 fn_801070F4(u32 a);
extern u8 lbl_8026768C[];
#if 0
asm void fn_80055E38(void) {
#include "src/game/scene_init_fn_80055E38.inc"
}
#else
#pragma optimization_level 4
u32 fn_80055E38(s32 idx) {
    u32 tbl[3];
    s32 val;
    u32 r;
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    if (idx < 0 || idx >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[idx];
    }
    if (val < 0) {
        return 1;
    }
    r = fn_801070F4((u32)val);
    return ((u8)r == 0) ? 1 : 0;
}
#endif

/* fn_80055EB8 - 0x80055EB8 | size: 0xd0 */
#if 0
asm void fn_80055EB8(void) {
#include "src/game/scene_init_fn_80055EB8.inc"
}
#else
u32 fn_80055EB8(u8* ctx, u8* p) {
    u8 result;
    u32 out;
    s16 x;
    s16 y;
    s16 idx;

    result = 0;
    if (fn_80057A08() != 0) {
        ctx = (u8*)fn_80104704(0x93);
        if (ctx != NULL) {
            if (fn_80055194(&out, (s8)ctx[0x95]) == 0) {
                result = 1;
                fn_80057094(&x, &y);
                idx = *(s16*)(p + 6);
                *(s16*)(p + 0x50) = (s16)(x + *(s16*)(lbl_802EF0A8 + idx * 0x1c + 2));
                *(s16*)(p + 0x52) = (s16)(y + *(s16*)(lbl_802EF0A8 + idx * 0x1c + 4));
            }
        }
    }
    fn_80109220((u32)p, result);
    return 0;
}
#endif

/* fn_80055F88 - 0x80055F88 | size: 0xfc */
extern u8 lbl_80267680[];
#if 0
asm void fn_80055F88(void) {
#include "src/game/scene_init_fn_80055F88.inc"
}
#else
u32 fn_80055F88(u8* unused, u8* p) {
    s32 idx;
    s16 val;
    void* obj;
    s16 width;
    s16 half;

    val = *(s16*)(p + 6);
    if (val == (s32)((u32*)lbl_80267680)[0]) {
        idx = 0;
    } else if (val == (s32)((u32*)lbl_80267680)[1]) {
        idx = 1;
    } else if (val == (s32)((u32*)lbl_80267680)[2]) {
        idx = 2;
    } else {
        idx = -1;
    }
    if (idx < 0) {
        return 0;
    }
    obj = fn_80134A98(NULL, (s8)idx);
    if (obj == NULL) {
        return 0;
    }
    fn_80132A38(0x37, (u32)obj);
    width = (s16)(fn_800FA444(0xce) >> 16);
    half = *(s16*)(p + 0x54);
    fn_800FB680((s16)(half / 2 - width / 2), 0, -1, 0xce);
    return 0;
}
#endif

/* fn_80056084 - 0x80056084 | size: 0x58c */
extern u8 lbl_80267518[];
extern f32 lbl_8047A574;
extern u32 lbl_8047BEB8;
extern u32 lbl_8047BEB0;
extern f32 lbl_8047BEB4;
extern u32 lbl_8047BEBC;
extern f32 lbl_8047A570;
extern u32 lbl_8047A56C;
extern u32 lbl_8047BEA8;
extern u32 lbl_8047BEA0;
extern f32 lbl_8047BEC0;
#if 1
asm void fn_80056084(void) {
#include "src/game/scene_init_fn_80056084.inc"
}
#else
void fn_80056084(void) { /* TODO */ }
#endif

/* fn_80056610 - 0x80056610 | size: 0xa4 */
extern u32 lbl_8047A568;
#if 0
asm void fn_80056610(void) {
#include "src/game/scene_init_fn_80056610.inc"
}
#else
#pragma optimization_level 4
void fn_80056610(u8* p) {
    s8 state;
    state = (s8)p[1];
    if (state == 3) {
        if ((s8)p[2] == 0) {
            fn_801080CC(*(u32*)(p + 4), 0x10b);
            p[2] = 1;
        }
    } else if (state == 0) {
        if ((s8)p[2] == 0) {
            if (lbl_8047A568 != 0) {
                fn_801080CC(*(u32*)(p + 4), 0x107);
            }
            p[2] = 1;
        }
    }
}
#endif

/* fn_800566B4 - 0x800566B4 | size: 0x24 */
extern f32 lbl_8047A570;
extern f32 lbl_8047BEC4;
#if 0
asm void fn_800566B4(void) {
#include "src/game/scene_init_fn_800566B4.inc"
}
#else
#pragma optimization_level 4
u32 fn_800566B4(void) {
    return !(lbl_8047A570 >= lbl_8047BEC4);
}
#endif

/* fn_800566D8 - 0x800566D8 | size: 0x10 */
extern f32 lbl_8047BEC0;
extern u32 lbl_8047A56C;
extern f32 lbl_8047A570;
#if 0
asm void fn_800566D8(void) {
#include "src/game/scene_init_fn_800566D8.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma optimization_level 4
void fn_800566D8(u32 a) {
    lbl_8047A56C = a;
    lbl_8047A570 = lbl_8047BEC0;
}
#pragma pop
#endif

/* fn_800566E8 - 0x800566E8 | size: 0x1c */
extern f32 lbl_8047BEC0;
extern f32 lbl_8047A578;
#if 0
asm void fn_800566E8(void) {
#include "src/game/scene_init_fn_800566E8.inc"
}
#else
#pragma optimization_level 4
u32 fn_800566E8(void) {
    return lbl_8047BEC0 != lbl_8047A578;
}
#endif

/* fn_80056704 - 0x80056704 | size: 0xa8 */
extern void fn_80102254(u32 a, u32 b);
extern u32 lbl_8047A584;
extern u32 lbl_8047A580;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047BEC8;
extern f32 lbl_8047A57C;
extern f32 lbl_8047A578;
#if 0
asm void fn_80056704(void) {
#include "src/game/scene_init_fn_80056704.inc"
}
#else
#pragma optimization_level 4
void fn_80056704(void) {
    u32 tbl[3];
    u32 cur;
    s32 val;
    cur = lbl_8047A584;
    lbl_8047A580 = cur;
    cur = cur - 1;
    lbl_8047A584 = cur;
    if ((s32)cur < 0) {
        cur = 2;
        lbl_8047A584 = 2;
    }
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    if ((s32)cur < 0 || (s32)cur >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[cur];
    }
    if (val >= 0) {
        fn_80102254((u32)val, 1);
    }
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BEC8;
}
#endif

/* fn_800567AC - 0x800567AC | size: 0xa8 */
extern u32 lbl_8047A584;
extern u32 lbl_8047A580;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047BECC;
extern f32 lbl_8047A57C;
extern f32 lbl_8047A578;
#if 0
asm void fn_800567AC(void) {
#include "src/game/scene_init_fn_800567AC.inc"
}
#else
#pragma optimization_level 4
void fn_800567AC(void) {
    u32 tbl[3];
    u32 cur;
    s32 val;
    cur = lbl_8047A584;
    lbl_8047A580 = cur;
    cur = cur + 1;
    lbl_8047A584 = cur;
    if ((s32)cur >= 3) {
        cur = 0;
        lbl_8047A584 = 0;
    }
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    if ((s32)cur < 0 || (s32)cur >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[cur];
    }
    if (val >= 0) {
        fn_80102254((u32)val, 1);
    }
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BECC;
}
#endif

/* fn_80056854 - 0x80056854 | size: 0x224 */
extern f32 lbl_8047A578;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047A57C;
extern f32 lbl_8047BEB4;
extern f32 lbl_8047BED0;
extern u32 lbl_8047A580;
extern f32 lbl_8047BED4;
extern u32 lbl_8047A584;
extern f32 lbl_8047A574;
extern f32 lbl_8047BED8;
extern f32 lbl_8047A570;
#if 0
asm void fn_80056854(void) {
#include "src/game/scene_init_fn_80056854.inc"
}
#else
void fn_80056854(void) {
    f32 old578;
    u32 tbl1[3];
    u32 tbl2[3];

    old578 = lbl_8047A578;
    if (old578 > lbl_8047BEC0) {
        lbl_8047A57C = lbl_8047A57C + old578;
        if (lbl_8047A57C >= lbl_8047BEB4) {
            lbl_8047A57C = lbl_8047BEB4;
            lbl_8047A578 = lbl_8047BEC0;
        }
    }
    if (lbl_8047A578 < lbl_8047BEC0) {
        lbl_8047A57C = lbl_8047A57C + lbl_8047A578;
        if (lbl_8047A57C <= lbl_8047BED0) {
            lbl_8047A57C = lbl_8047BED0;
            lbl_8047A578 = lbl_8047BEC0;
        }
    }
    if (lbl_8047BEC0 != old578) {
        s32 val;
        u32 idx = lbl_8047A580;
        tbl1[0] = ((u32*)lbl_8026768C)[0];
        tbl1[1] = ((u32*)lbl_8026768C)[1];
        tbl1[2] = ((u32*)lbl_8026768C)[2];
        if ((s32)idx < 0 || (s32)idx >= 3) {
            val = -1;
        } else {
            val = (s32)tbl1[idx];
        }
        if (val >= 0) {
            if (lbl_8047BEC0 == lbl_8047A578) {
                fn_80102254((u32)val, 0);
            } else {
                void* obj = fn_80104704((u32)val);
                if (obj != NULL) {
                    *(s16*)((u8*)obj + 0x84) = (s16)(s32)(lbl_8047BED4 * lbl_8047A57C);
                }
            }
        }
        idx = lbl_8047A584;
        {
            u32 *src = (u32*)&lbl_8026768C;
            tbl2[0] = src[0];
            tbl2[1] = src[1];
            tbl2[2] = src[2];
        }
        if ((s32)idx < 0 || (s32)idx >= 3) {
            val = -1;
        } else {
            val = (s32)tbl2[idx];
        }
        if (val >= 0) {
            void* obj = fn_80104704((u32)val);
            if (obj != NULL) {
                *(s16*)((u8*)obj + 0x84) = (s16)(s32)(lbl_8047BED4 * lbl_8047A57C);
                if (old578 > lbl_8047BEC0) {
                    *(s16*)((u8*)obj + 0x84) = *(s16*)((u8*)obj + 0x84) + 0x1a6;
                } else {
                    *(s16*)((u8*)obj + 0x84) = *(s16*)((u8*)obj + 0x84) - 0x1a6;
                }
            }
        }
    }
    lbl_8047A574 = lbl_8047A574 + lbl_8047BED8;
    if (lbl_8047A574 > lbl_8047BEB4) {
        lbl_8047A574 = lbl_8047A574 - lbl_8047BEB4;
    }
    if (lbl_8047A570 < lbl_8047BEB4) {
        lbl_8047A570 = lbl_8047A570 + lbl_8047BED8;
        if (lbl_8047A570 >= lbl_8047BEB4) {
            lbl_8047A570 = lbl_8047BEB4;
        }
    }
}
#endif

/* fn_80056A80 - 0x80056A80 | size: 0xf4 */
extern u32 lbl_8047A584;
#if 0
asm void fn_80056A80(void) {
#include "src/game/scene_init_fn_80056A80.inc"
}
#else
void fn_80056A80(void) {
    s32 i;
    s32 val;
    u32 tbl[3];

    for (i = 0; i < 3; i++) {
        tbl[0] = ((u32*)lbl_8026768C)[0];
        tbl[1] = ((u32*)lbl_8026768C)[1];
        tbl[2] = ((u32*)lbl_8026768C)[2];
        if (i < 0 || i >= 3) {
            val = -1;
        } else {
            val = (s32)tbl[i];
        }
        if (val >= 0) {
            fn_80102568(val, 2, 0);
        }
    }
    {
        u32 tbl2[3];
        s32 idx;
        tbl2[0] = ((u32*)lbl_8026768C)[0];
        tbl2[1] = ((u32*)lbl_8026768C)[1];
        tbl2[2] = ((u32*)lbl_8026768C)[2];
        idx = (s32)lbl_8047A584;
        if (idx < 0 || idx >= 3) {
            val = -1;
        } else {
            val = (s32)tbl2[idx];
        }
        if (val >= 0) {
            menuCloseSync(val, 1);
        }
    }
}
#endif

/* fn_80056B74 - 0x80056B74 | size: 0xe0 */
extern u32 lbl_8047A568;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047BEB4;
extern u32 lbl_8047A584;
extern u32 lbl_8047A580;
extern f32 lbl_8047A57C;
extern f32 lbl_8047A578;
extern f32 lbl_8047A574;
extern f32 lbl_8047A570;
#if 0
asm void fn_80056B74(u32 a, s32 b) {
#include "src/game/scene_init_fn_80056B74.inc"
}
#else
void fn_80056B74(u32 idx, s32 mode) {
    s32 i;
    s32 val;
    u32 tbl[3];

    lbl_8047A568 = (u32)mode;
    for (i = 0; i < 3; i++) {
        tbl[0] = ((u32*)lbl_8026768C)[0];
        tbl[1] = ((u32*)lbl_8026768C)[1];
        tbl[2] = ((u32*)lbl_8026768C)[2];
        if (i < 0 || i >= 3) {
            val = -1;
        } else {
            val = (s32)tbl[i];
        }
        if (val >= 0) {
            fn_801026A4(val, 0x1f, 0, 0, 0, 0);
        }
        if (idx != (u32)i) {
            fn_80102254(val, 0);
        }
    }
    lbl_8047A584 = idx;
    lbl_8047A580 = idx;
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BEC0;
    lbl_8047A574 = lbl_8047BEC0;
    lbl_8047A570 = lbl_8047BEB4;
}
#endif

/* fn_80057094 - 0x80057094 | size: 0x3c */
extern u8 lbl_803A9768[];
#if 0
asm void fn_80057094(void) {
#include "src/game/scene_init_fn_80057094.inc"
}
#else
#pragma optimization_level 4
void fn_80057094(s16* a, s16* b) {
    *a = (s16)(s32)*(f32*)(lbl_803A9768 + 0x27c);
    *b = (s16)(s32)*(f32*)(lbl_803A9768 + 0x280);
}
#endif

/* fn_800570D0 - 0x800570D0 | size: 0x44 */
extern void fn_80056C54(u8* a, u8* b, u32 c);
#if 0
asm void fn_800570D0(void) {
#include "src/game/scene_init_fn_800570D0.inc"
}
#else
#pragma optimization_level 4
u32 fn_800570D0(u8* a, u8* b) {
    s32 c;
    c = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1);
    fn_80056C54(a, b, (u32)(c % 2));
    return 0;
}
#endif

/* fn_80057114 - 0x80057114 | size: 0x30 */
#if 0
asm void fn_80057114(void) {
#include "src/game/scene_init_fn_80057114.inc"
}
#else
#pragma optimization_level 4
u32 fn_80057114(u8* a, u8* b) {
    fn_80056C54(a, b, *(u32*)(lbl_803A9768 + 0x278));
    return 0;
}
#endif

/* fn_80057144 - 0x80057144 | size: 0x12c */
extern u8 lbl_802676B4[];
/* Model visibility check with Tbl14 table lookup + position update. */
#if 0
asm void fn_80057144(void) {
#include "src/game/scene_init_fn_80057144.inc"
}
#else
#pragma optimization_level 4
u32 fn_80057144(u8* ctx, u8* p) {
    u32 state;
    u32 field4;
    Tbl14 tbl;
    u16 id;
    s16 field6;

    state = *(u32*)(lbl_803A9768);
    field6 = *(s16*)(p + 6);
    field4 = *(u32*)(lbl_803A9768 + 4);
    tbl = *(Tbl14*)lbl_802676B4;
    id = (u16)tbl.data[(state * 2 + (field4 != 0 ? 1 : 0))];
    if (field6 == (s16)id) {
        fn_80109220((u32)p, 1);
    } else {
        fn_80109220((u32)p, 0);
    }
    *(s16*)(ctx + 0x84) = (s16)(s32)*(f32*)(lbl_803A9768 + 0x27c);
    *(s16*)(ctx + 0x86) = (s16)(s32)*(f32*)(lbl_803A9768 + 0x280);
    return 0;
}
#endif

/* fn_800573C0 - 0x800573C0 | size: 0x40 */
extern f32 lbl_8047BF00;
#if 0
asm void fn_800573C0(void) {
#include "src/game/scene_init_fn_800573C0.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_800573C0(void) {
    s32 v;
    if (*(f32*)(lbl_803A9768 + 0x288) <= lbl_8047BF00) {
        v = *(s32*)lbl_803A9768;
        if (v == 0 || v == 3) {
            return 0;
        }
    }
    return 1;
}
#endif

/* fn_80057400 - 0x80057400 | size: 0x28 */
#if 0
asm void fn_80057400(void) {
#include "src/game/scene_init_fn_80057400.inc"
}
#else
#pragma optimization_level 4
void fn_80057400(void) {
    s32 v = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1);
    *(u32*)(lbl_803A9768 + 0x278) = v;
    if (v > 1) {
        *(u32*)(lbl_803A9768 + 0x278) = 0;
    }
}
#endif

/* fn_80057428 - 0x80057428 | size: 0x24 */
extern f32 lbl_8047A588;
extern f32 lbl_8047BF04;
#if 0
asm void fn_80057428(void) {
#include "src/game/scene_init_fn_80057428.inc"
}
#else
#pragma optimization_level 4
u32 fn_80057428(void) {
    return !(lbl_8047A588 >= lbl_8047BF04);
}
#endif

/* fn_8005744C - 0x8005744C | size: 0xc */
extern f32 lbl_8047BF00;
extern f32 lbl_8047A588;
#if 0
asm void fn_8005744C(void) {
#include "src/game/scene_init_fn_8005744C.inc"
}
#else
#pragma optimization_level 4
void fn_8005744C(void) {
    lbl_8047A588 = lbl_8047BF00;
}
#endif

/* fn_80057458 - 0x80057458 | size: 0x50 */
/* Copy 0x138 bytes from src to next slot ((slot+1) % 2). */
#if 0
asm void fn_80057458(void) {
#include "src/game/scene_init_fn_80057458.inc"
}
#else
#pragma optimization_level 4
void fn_80057458(u8* src) {
    s32 next = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1) % 2;
    Tbl78* dstState = (Tbl78*)(lbl_803A9768 + next * 0x138 + 8);
    Tbl78* srcState = (Tbl78*)src;

    *dstState = *srcState;
}
#endif

/* fn_800574A8 - 0x800574A8 | size: 0x38 */
#if 0
asm void fn_800574A8(void) {
#include "src/game/scene_init_fn_800574A8.inc"
}
#else
#pragma push
#pragma scheduling off
#pragma optimization_level 4
void fn_800574A8(void) {
    fn_80124A60(lbl_803A9768 + *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8);
}
#pragma pop
#endif

/* fn_800574E0 - 0x800574E0 | size: 0x1c */
#if 0
asm void fn_800574E0(void) {
#include "src/game/scene_init_fn_800574E0.inc"
}
#else
#pragma optimization_level 4
u8* fn_800574E0(void) {
    return lbl_803A9768 + *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8;
}
#endif

/* fn_800574FC - 0x800574FC | size: 0x3c */
/* Block copy 0x138 bytes from src to current slot.
 * Uses unrolled 2-word copy loop (39 iterations). */
#if 0
asm void fn_800574FC(void) {
#include "src/game/scene_init_fn_800574FC.inc"
}
#else
#pragma optimization_level 4
void fn_800574FC(u8* src) {
    u32 slot = *(u32*)(lbl_803A9768 + 0x278);
    Tbl78* dstState = (Tbl78*)(lbl_803A9768 + slot * 0x138 + 8);
    Tbl78* srcState = (Tbl78*)src;

    *dstState = *srcState;
}
#endif

/* fn_80057538 - 0x80057538 | size: 0x15c */
extern u8 fn_80107170(u32 scene, u16 id);
extern u8 lbl_80267698[];
/* Scene visibility check using lbl_802676B4 table.
 * state 6 = special case for 0x76e/0x77e scene objects. */
#if 0
asm void fn_80057538(void) {
#include "src/game/scene_init_fn_80057538.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
u32 fn_80057538(void) {
    s32 state;
    u32 field4;
    s32 count;
    Tbl14 tbl;
    u16 id;
    u8* tbl_base;

    state = *(s32*)(lbl_803A9768);
    if (state != 6) {
        count = *(s32*)(lbl_80267698 + state * 4);
        if (count <= 0) { return 1; }
        field4 = *(u32*)(lbl_803A9768 + 4);
        tbl_base = lbl_802676B4;
        tbl = *(Tbl14*)tbl_base;
        id = (u16)tbl.data[(state * 2 + (field4 != 0 ? 1 : 0))];
        if ((u8)fn_80107170(0xa0, id) != 0) { return 0; }
        return 1;
    }
    if ((u8)fn_80107170(0xa0, 0x76e) != 0) { return 0; }
    if ((u8)fn_80107170(0xa0, 0x77e) != 0) { return 0; }
    return 1;
}
#pragma peephole on
#endif

/* fn_80057694 - 0x80057694 | size: 0x10 */
#if 0
asm void fn_80057694(void) {
#include "src/game/scene_init_fn_80057694.inc"
}
#else
#pragma optimization_level 4
u32 fn_80057694(void) {
    return *(u32*)(lbl_803A9768 + 4);
}
#endif

/* fn_800576A4 - 0x800576A4 | size: 0x10 */
#if 0
asm void fn_800576A4(void) {
#include "src/game/scene_init_fn_800576A4.inc"
}
#else
#pragma optimization_level 4
void fn_800576A4(u32 a) {
    *(u32*)(lbl_803A9768 + 4) = a;
}
#endif

/* fn_800576B4 - 0x800576B4 | size: 0x10 */
#if 0
asm void fn_800576B4(void) {
#include "src/game/scene_init_fn_800576B4.inc"
}
#else
#pragma optimization_level 4
u32 fn_800576B4(void) {
    return *(u32*)(lbl_803A9768 + 0);
}
#endif

/* fn_800576C4 - 0x800576C4 | size: 0x16c */
extern void fn_801081F8(u8* obj, u16 id, u16 count);
/* Set scene models based on state. Uses Tbl14 table for ID lookup. */
#if 0
asm void fn_800576C4(void) {
#include "src/game/scene_init_fn_800576C4.inc"
}
#else
#pragma optimization_level 4
void fn_800576C4(u32 state) {
    u8* obj;
    s32 count;
    u32 field4;
    Tbl14 tbl;
    u16 id;

    *(u32*)(lbl_803A9768) = state;
    obj = fn_80104704(0xa0);
    count = *(s32*)(lbl_80267698 + state * 4);
    if (count <= 0) { return; }
    if (obj == NULL) { return; }
    field4 = *(u32*)(lbl_803A9768 + 4);
    tbl = *(Tbl14*)lbl_802676B4;
    id = (u16)tbl.data[(state * 2 + (field4 != 0 ? 1 : 0))];
    fn_801081F8(obj, id, (u16)count);
    if (state == 2 || state == 4) {
        if (count > 0) {
            fn_801081F8(obj, 0x76e, (u16)count);
        }
    }
    if (state == 6) {
        fn_801081F8(obj, 0x76e, 0xe5);
        fn_801081F8(obj, 0x77e, 0xec);
    }
}
#endif

/* fn_80057830 - 0x80057830 | size: 0x118 */
extern f64 lbl_8047BEE8;
extern f32 lbl_8047BEF4;
extern f32 lbl_8047BF00;
extern f32 lbl_8047BF08;
/* Set camera target positions from integer coords.
 * c != 0: full reset (set all 6 fields from a,b).
 * c == 0: transition (save current, set target to a,b). */
#if 0
asm void fn_80057830(s32 a, s32 b, s32 c) {
#include "src/game/scene_init_fn_80057830.inc"
}
#else
#pragma optimization_level 4
void fn_80057830(s32 a, s32 b, s32 c) {
    u8* base = (u8*)lbl_803A9768;
    if (c != 0) {
        *(f32*)(base + 0x27c) = (f32)(s32)a;
        *(f32*)(base + 0x280) = (f32)(s32)b;
        *(f32*)(base + 0x294) = (f32)(s32)a;
        *(f32*)(base + 0x298) = (f32)(s32)b;
        *(f32*)(base + 0x28c) = (f32)(s32)a;
        *(f32*)(base + 0x290) = (f32)(s32)b;
        *(f32*)(base + 0x284) = lbl_8047BEF4;
        *(f32*)(base + 0x288) = lbl_8047BF00;
    } else {
        *(f32*)(base + 0x28c) = *(f32*)(base + 0x27c);
        *(f32*)(base + 0x290) = *(f32*)(base + 0x280);
        *(f32*)(base + 0x294) = (f32)(s32)a;
        *(f32*)(base + 0x298) = (f32)(s32)b;
        *(f32*)(base + 0x284) = lbl_8047BF00;
        *(f32*)(base + 0x288) = lbl_8047BF08;
    }
}
#endif

/* fn_80057948 - 0x80057948 | size: 0xc0 */
extern f32 lbl_8047BF00;
extern f32 lbl_8047BEF4;
extern f32 lbl_8047A58C;
extern f32 lbl_8047BF0C;
extern f32 lbl_8047A588;
#if 0
asm void fn_80057948(void) {
#include "src/game/scene_init_fn_80057948.inc"
}
#else
#pragma optimization_level 4
void fn_80057948(void) {
    f32 f0;
    f32 f1;
    f32 f2;
    f32 f3;
    f32 f4;
    f2 = lbl_8047BF00;
    f3 = *(f32*)(lbl_803A9768 + 0x288);
    if (f3 > f2) {
        f1 = *(f32*)(lbl_803A9768 + 0x284) + f3;
        f0 = lbl_8047BEF4;
        *(f32*)(lbl_803A9768 + 0x284) = f1;
        if (f1 < f0) {
            *(f32*)(lbl_803A9768 + 0x284) = f0;
            *(f32*)(lbl_803A9768 + 0x288) = lbl_8047BF00;
        }
        f2 = *(f32*)(lbl_803A9768 + 0x284);
        f3 = *(f32*)(lbl_803A9768 + 0x28c);
        f1 = *(f32*)(lbl_803A9768 + 0x294);
        f4 = *(f32*)(lbl_803A9768 + 0x290);
        f0 = *(f32*)(lbl_803A9768 + 0x298);
        *(f32*)(lbl_803A9768 + 0x27c) = f2 * (f1 - f3) + f3;
        *(f32*)(lbl_803A9768 + 0x280) = f2 * (f0 - f4) + f4;
    }
    f2 = lbl_8047A58C;
    f1 = f2 + lbl_8047BF0C;
    lbl_8047A58C = f1;
    if (f1 > lbl_8047BEF4) {
        lbl_8047A58C = f1 - lbl_8047BEF4;
    }
    if (lbl_8047A588 >= lbl_8047BEF4) { return; }
    f0 = lbl_8047A588 + lbl_8047BF0C;
    lbl_8047A588 = f0;
    if (f0 <= lbl_8047BEF4) { return; }
    lbl_8047A588 = lbl_8047BEF4;
}
#endif

/* fn_80057A08 - 0x80057A08 | size: 0x30 */
#if 0
asm void fn_80057A08(void) {
#include "src/game/scene_init_fn_80057A08.inc"
}
#else
#pragma push
#pragma scheduling off
#pragma optimization_level 4
u32 fn_80057A08(void) {
    return (u32)fn_80104704(0xa0) != 0;
}
#pragma pop
#endif

/* fn_80057A38 - 0x80057A38 | size: 0x2c */
#if 0
asm void fn_80057A38(void) {
#include "src/game/scene_init_fn_80057A38.inc"
}
#else
#pragma push
#pragma scheduling off
#pragma optimization_level 4
void fn_80057A38(void) {
    fn_80102568(0xa0, 2, 1);
}
#pragma pop
#endif

/* fn_80057A64 - 0x80057A64 | size: 0xd0 */
extern f32 lbl_8047BF00;
extern f32 lbl_8047BEF4;
extern f32 lbl_8047A58C;
extern f32 lbl_8047A588;
/* Init camera system: clear both slots, optionally load state, start scene 0xa0. */
#if 0
asm void fn_80057A64(s32 a, s32 b) {
#include "src/game/scene_init_fn_80057A64.inc"
}
#else
#pragma optimization_level 4
void fn_80057A64(u8* state, u32 b) {
    u8* base;
    s32 i = 0;

    base = (u8*)lbl_803A9768;
    *(u32*)(base + 0x278) = i;
    *(u32*)(base + 4) = b;
    while (i < 2) {
        fn_80124A60(base + 8);
        base += 0x138;
        i++;
    }
    if (state != NULL) {
        Tbl78* dstState;
        Tbl78* srcState;

        base = (u8*)lbl_803A9768;
        *(u32*)(base + 0) = 3;
        dstState = (Tbl78*)(base + 8);
        srcState = (Tbl78*)state;
        *dstState = *srcState;
    }
    lbl_8047A58C = lbl_8047BF00;
    lbl_8047A588 = lbl_8047BEF4;
    fn_801026A4(0xa0, 0x1f, 0, 0, 0, 0);
}
#endif

/* fn_80057C9C - 0x80057C9C | size: 0x14c */
extern void fn_80097D94();
extern u8 lbl_803A9A08[];
#if 0
asm void fn_80057C9C(void) {
#include "src/game/scene_init_fn_80057C9C.inc"
}
#else
void fn_80057C9C(u32 a, u32 b, u32 c) {
    u8 waited;
    s32 status;

    waited = 0;
    if (*(u32*)lbl_803A9A08 != 0) {
        if (*(u32*)(lbl_803A9A08 + 4) != 0) {
            status = 1;
        } else {
            status = 0;
        }
    } else {
        status = 2;
    }
    if (status != 2) {
        waited = 1;
        *(u32*)(lbl_803A9A08 + 8) = 1;
        do {
            if (*(u32*)lbl_803A9A08 != 0) {
                if (*(u32*)(lbl_803A9A08 + 4) != 0) {
                    status = 1;
                } else {
                    status = 0;
                }
            } else {
                status = 2;
            }
            if (status == 1) { break; }
            _threadSwitch();
        } while (1);
    }
    fn_8017B1CC(0x70b);
    fn_800F915C(0x70b);
    fn_8017B3E4(0x66f);
    do {
        if (fn_8017B2CC(0x66f) != 1) { break; }
        _threadSwitch();
    } while (1);
    fn_80097D94(a, b, c);
    fn_8017B1CC(0x66f);
    fn_800F915C(0x66f);
    fn_8017B3E4(0x70b);
    do {
        if (fn_8017B2CC(0x70b) != 1) { break; }
        _threadSwitch();
    } while (1);
    if ((u8)waited != 0) {
        *(u32*)(lbl_803A9A08 + 8) = 0;
    }
}
#endif

/* fn_80057DE8 - 0x80057DE8 | size: 0x58 */
extern u8 fn_8011FC74(u32 a);
extern u8 fn_80123FBC(void* a);
#if 0
asm void fn_80057DE8(void) {
#include "src/game/scene_init_fn_80057DE8.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_80057DE8(u32 a) {
    if ((u8)fn_80123FBC((void*)a) == 0) { return 0; }
    return (u8)fn_8011FC74(a) != 0 ? 1 : 0;
}
#pragma scheduling off
#endif

/* fn_80057E40 - 0x80057E40 | size: 0x30 */
#if 0
asm s32 fn_80057E40(void) {
#include "src/game/scene_init_fn_80057E40.inc"
}
#else
#pragma optimization_level 4
s32 fn_80057E40(void) {
    if (*(u32*)(lbl_803A9A08 + 0) == 0) { return 2; }
    return *(u32*)(lbl_803A9A08 + 4) != 0 ? 1 : 0;
}
#endif

/* fn_80057E70 - 0x80057E70 | size: 0x124 */
extern u8 fn_8017B13C(u32 id, u32 val);
extern u32 lbl_8047A590;
/* BGM queue player: iterate null-terminated u32 list, play each via 0x48a.
 * r26=firstIter, r27=listPtr, r28=&base[8], r29=&base[4], r30=base, r31=1 */
#if 0
asm void fn_80057E70(void) {
#include "src/game/scene_init_fn_80057E70.inc"
}
#else
#pragma optimization_level 4
void fn_80057E70(void) {
    u32* list = (u32*)lbl_8047A590;
    u8* base = (u8*)lbl_803A9A08;
    u8 firstIter = 1;

    *(u32*)(base + 0) = 1;
    *(u32*)(base + 8) = 0;
    *(u32*)(base + 4) = 0;
    *(u32*)(base + 0xc) = 0;

    if (list == NULL) { goto done; }

    while (*list != 0 && *(u32*)(base + 0xc) == 0) {
        if ((u8)firstIter == 0) {
            if ((u8)fn_8017B07C(0x48a) != 0) {
                goto next;
            }
        }
        if ((u8)fn_8017B13C(0x48a, *list) == 0) {
            goto next;
        }
        firstIter = 0;
        while (fn_8017B2CC(0x48a) == 1) {
            _threadSwitch();
        }
        do {
            *(u32*)(base + 4) = 1;
            _threadSwitch();
        } while (*(u32*)(base + 8) != 0);
        *(u32*)(base + 4) = 0;
    next:
        list++;
    }

done:
    {
        u8* b = (u8*)lbl_803A9A08;
        *(u32*)(b + 0) = 0;
        if (*(u32*)(base + 0xc) != 0) {
            fn_8017B1CC(0x48a);
            fn_800F915C(0x48a);
        }
    }
}
#endif

/* fn_80057F94 - 0x80057F94 | size: 0x1bc */
extern u8 fn_80122334(void);
extern u8 lbl_802676F0[];
/* BGM lookup: resolve bgmId to sound ID via table, play if ready.
 * Uses Tbl56 for 0xE0-byte table copy from lbl_802676F0. */
#if 0
asm void fn_80057F94(void) {
#include "src/game/scene_init_fn_80057F94.inc"
}
#else
#pragma optimization_level 4
u32 fn_80057F94(u32 bgmId) {
    u16 sndId;
    Tbl56 tbl;
    u8 idx;

    if ((u8)fn_80123FBC((void*)bgmId) == 0) { return 0; }
    sndId = (u16)fn_8011F5C8(bgmId);
    if ((u16)sndId == 0) {
        sndId = (u16)-1;
    } else if ((u16)sndId == 0xc9) {
        tbl = *(Tbl56*)lbl_802676F0;
        fn_8012640C(bgmId, (u16)sndId, 0x6f, 0);
        idx = (u8)fn_80122334();
        if ((u8)idx >= 0x1c) { sndId = (u16)-1; goto check; }
        sndId = (u16)fn_8011F5C8(bgmId);
        if ((u16)sndId == 0) { sndId = (u16)-1; goto check; }
        if ((u8)fn_8012640C(bgmId, (u16)sndId, 0xc1, 0) != 0) {
            sndId = (u16)tbl.data[((u8)idx * 2 + 1)];
        } else {
            sndId = (u16)tbl.data[((u8)idx * 2)];
        }
    } else {
        u16 flag;
        flag = (u16)((u8)fn_8012640C(bgmId, sndId, 0xc1, 0) != 0 ? 1 : 0);
        sndId = (u16)fn_8012640C(0, sndId, 0x5a, flag);
        if ((u16)sndId == 0) {
            sndId = (u16)-1;
        }
    }
check:
    if ((u16)(sndId + 1) == 0xFFFF) { return 0; }
    if ((u8)fn_8017B07C(0x48a, sndId) == 0) { return 0; }
    return fn_800F92D4(sndId);
}
#endif

/* fn_80058150 - 0x80058150 | size: 0x604 */
extern void fn_801CAC6C(void);
extern void fn_800E3534(void);
extern void fn_800FF560(void);
extern void GSthreadCreate(void);
extern void fn_801CAAF4(void);
extern void fn_80190528(void);
extern u32 lbl_8047A590;
#if 1
asm void fn_80058150(void) {
#include "src/game/scene_init_fn_80058150.inc"
}
#else
void fn_80058150(void) { /* TODO */ }
#endif

/* fn_80058754 - 0x80058754 | size: 0x44 */
extern u8 lbl_803A9A18[];
#if 0
asm void fn_80058754(void) {
#include "src/game/scene_init_fn_80058754.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_80058754(u32 unused, u8* p) {
    fn_80132A38(0x37, (u32)lbl_803A9A18);
    *(u32*)(p + 0x4c) = 0xce;
    return 0;
}
#pragma scheduling off
#endif

/* fn_80058798 - 0x80058798 | size: 0x40 */
#if 0
asm void fn_80058798(void) {
#include "src/game/scene_init_fn_80058798.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
void fn_80058798(u8* ctx) {
    u8* s;
    s = fn_80105624();
    if (*(u16*)(s + 4) != 0) {
        ctx[0x98] = 1;
    }
}
#pragma scheduling off
#endif

/* fn_800587D8 - 0x800587D8 | size: 0x2c */
#if 0
asm void fn_800587D8(void) {
#include "src/game/scene_init_fn_800587D8.inc"
}
#else
#pragma optimization_level 4
void fn_800587D8(void) {
    fn_80102568(0x9b, 2, 1);
}
#endif

/* fn_80058804 - 0x80058804 | size: 0x7c */
#if 0
asm void fn_80058804(u8* a, s32 b) {
#include "src/game/scene_init_fn_80058804.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
void fn_80058804(u8* a, s32 b) {
    if (a == 0) {
        *(u16*)lbl_803A9A18 = 0;
    } else {
        fn_800F96E4(lbl_803A9A18, 0x21, a);
    }
    fn_8010264C(0x9b, (u8)b);
    if (b != 0) {
        fn_80102568(0x9b, 2, 1);
    }
}
#pragma pop
#endif

/* fn_80058880 - 0x80058880 | size: 0x230 */
extern u8 lbl_802677D0[];
extern u32 lbl_8047A598;
extern u32 lbl_8047A59C;
#if 0
asm void fn_80058880(void) {
#include "src/game/scene_init_fn_80058880.inc"
}
#else
#pragma peephole off
u32 fn_80058880(u8* ctx) {
    u8* input;
    s8 cur;
    u16 buttons;
    s32 tblVal;

    input = fn_80105624();
    if (fn_800573C0() != 0) { return 0; }
    if (fn_800566E8() != 0) { return 0; }

    buttons = *(u16*)(input + 6);
    /* bits 6-7: left/right toggle */
    if (buttons & 0xC0) {
        if (fn_80057694() != 0) {
            fn_800576A4(0);
        } else {
            fn_800576A4(1);
        }
    }
    /* bit 1: down */
    if (buttons & 2) {
        cur = (s8)(ctx[0x95] + 1);
        ctx[0x95] = (u8)cur;
        if ((s32)cur >= 7) {
            ctx[0x95] = 0;
        }
    }
    /* bit 0: up */
    if (buttons & 1) {
        cur = (s8)(ctx[0x95] - 1);
        ctx[0x95] = (u8)cur;
        if ((s32)cur < 0) {
            ctx[0x95] = 6;
        }
    }
    /* button 8: A button */
    if (buttons == 8) {
        cur = (s8)ctx[0x95];
        if (cur < 0 || cur >= 7) {
            tblVal = 2;
        } else {
            tblVal = *(s32*)(lbl_802677D0 + (u32)cur * 0x10 + 0xc);
        }
        if (tblVal == 0) {
            ctx[0x95] = (u8)(s8)(s32)lbl_8047A598;
        } else if (tblVal == 1) {
            ctx[0x98] = 1;
            ctx[0x99] = 1;
        }
    }
    /* button 4: B button */
    if (buttons == 4) {
        cur = (s8)ctx[0x95];
        if (cur < 0 || cur >= 7) {
            tblVal = 2;
        } else {
            tblVal = *(s32*)(lbl_802677D0 + (u32)cur * 0x10 + 0xc);
        }
        if (tblVal == 1) {
            ctx[0x95] = (u8)(s8)(s32)lbl_8047A59C;
        }
    }
    /* update if changed */
    {
        s8 newCur = (s8)ctx[0x95];
        s8 oldCur = (s8)ctx[0x97];
        if (newCur != oldCur) {
            if (newCur < 0 || newCur >= 7) {
                tblVal = 2;
            } else {
                tblVal = *(s32*)(lbl_802677D0 + (u32)(s32)newCur * 0x10 + 0xc);
            }
            if (tblVal == 0) {
                lbl_8047A59C = (u32)newCur;
            }
            if (tblVal != 0) {
                lbl_8047A598 = (u32)newCur;
            }
            {
                u8* ptr = fn_80104318(ctx);
                fn_80057830(*(s16*)(ptr + 2), *(s16*)(ptr + 4), 0);
            }
        }
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_80058AB0 - 0x80058AB0 | size: 0x40 */
extern void fn_80102ED4(u8* ctx);
#if 0
asm void fn_80058AB0(void) {
#include "src/game/scene_init_fn_80058AB0.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
void fn_80058AB0(u8* ctx) {
    fn_80105624();
    if ((s32)fn_800573C0() == 0) {
        fn_80102ED4(ctx);
    }
}
#pragma scheduling off
#endif

/* fn_80058AF0 - 0x80058AF0 | size: 0x2dc */
#if 1
asm u32 fn_80058AF0(u8* ctx) {
#include "src/game/scene_init_fn_80058AF0.inc"
}
#else
u32 fn_80058AF0(u8* ctx) { /* TODO */ }
#endif

/* fn_80058DCC - 0x80058DCC | size: 0x13c */
#if 0
asm void fn_80058DCC(void) {
#include "src/game/scene_init_fn_80058DCC.inc"
}
#else
#pragma scheduling on
#pragma peephole off
s32 fn_80058DCC(u8* ctx) {
    s32 i;
    u8* ptr;
    u8* subctx;

    subctx = *(u8**)(ctx + 0x60);
    switch ((s8)ctx[1]) {
    case 0:
        if ((s8)ctx[2] == 0) {
            ctx[2] = 1;
        }
        if (*(s32*)subctx != 0) {
            i = 0;
            while (i < 6) {
                if ((u8)fn_80123FBC(fn_8012A5B0(0, 3, (u16)i)) == 0) {
                    break;
                }
                i++;
            }
            if (i >= 6) {
                i = -1;
            }
            if (i >= 0) {
                *(s8*)(ctx + 0x95) = (s8)i;
            }
        }
        ptr = fn_80104318(ctx);
        fn_80057830(*(s16*)(ptr + 2), *(s16*)(ptr + 4), 0);
        break;
    case 2:
        if ((s32)fn_801046B8() == *(s32*)(ctx + 4)) {
            if ((s32)fn_80058AF0(ctx) == 0) {
                ctx[0x98] = 1;
                ctx[0x99] = 1;
            }
        }
        break;
    case 3:
        if ((s8)ctx[2] == 0) {
            ctx[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma scheduling off
#pragma peephole on
#endif

/* fn_80058F08 - 0x80058F08 | size: 0x38 */
#if 0
asm void fn_80058F08(void) {
#include "src/game/scene_init_fn_80058F08.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
u32 fn_80058F08(u32* out, s32 idx) {
    if (idx < 0 || idx >= 7) { return 2; }
    *out = *(u32*)(lbl_802677D0 + idx * 0x10 + 0x8);
    return *(u32*)(lbl_802677D0 + idx * 0x10 + 0x4);
}
#pragma scheduling off
#endif

/* fn_80058F40 - 0x80058F40 | size: 0xf4 */
#if 0
asm void fn_80058F40(void) {
#include "src/game/scene_init_fn_80058F40.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
void* fn_80058F40(void* dflt, s32 dir, u32* state) {
    s32 i;
    void* found;

    if (*(s32*)state != 0) {
        return dflt;
    }
    if (dir == 1) {
        i = (s32)state[1] - 1;
        while (i >= 0) {
            found = fn_8012A5B0(0, 3, (u16)i);
            if ((u8)fn_80123FBC(found) != 0) {
                break;
            }
            i--;
        }
        if (i >= 0) {
            state[1] = (u32)i;
            return found;
        }
    }
    if (dir == 2) {
        i = (s32)state[1] + 1;
        while (i < 6) {
            found = fn_8012A5B0(0, 3, (u16)i);
            if ((u8)fn_80123FBC(found) != 0) {
                break;
            }
            i++;
        }
        if (i < 6) {
            state[1] = (u32)i;
            return found;
        }
    }
    return dflt;
}
#pragma peephole on
#pragma scheduling off
#endif

/* fn_800599AC - 0x800599AC | size: 0x230 */
extern u32 fn_80059034(u32 a);
extern u32 lbl_8047A59C;
extern u32 lbl_8047A598;
#if 0
asm void fn_800599AC(u32 param) {
#include "src/game/scene_init_fn_800599AC.inc"
}
#else
#pragma peephole off
void fn_800599AC(u32 param) {
    u32 r30;
    u32 r31;
    u32* r29;
    u32 buf[2];
    u8* tbl;
    s32 result;
    s32 tblVal;
    s32 tblVal2;

    tbl = (u8*)lbl_802677D0;
    buf[0] = 0;
    r30 = 0;

    /* Find first entry with field_c == 0 */
    {
        s32 idx = 0;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } tbl += 0x10; idx = 1;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } tbl += 0x10; idx = 2;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } tbl += 0x10; idx = 3;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } tbl += 0x10; idx = 4;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } tbl += 0x10; idx = 5;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } tbl += 0x10; idx = 6;
        if (*(s32*)(tbl + 0xc) == 0) { goto found0; } idx = 7;
    found0:
        lbl_8047A59C = (u32)idx;
    }

    /* Find first entry with field_c == 1 */
    {
        s32 idx = 0;
        tbl = (u8*)lbl_802677D0;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } tbl += 0x10; idx = 1;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } tbl += 0x10; idx = 2;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } tbl += 0x10; idx = 3;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } tbl += 0x10; idx = 4;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } tbl += 0x10; idx = 5;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } tbl += 0x10; idx = 6;
        if (*(s32*)(tbl + 0xc) == 1) { goto found1; } idx = 7;
    found1:
        lbl_8047A598 = (u32)idx;
    }

    r29 = &buf[0];
    buf[1] = param;
    do {
        result = fn_801026A4(0x94, (u32)fn_801046B8(), (u32)r29, 0, 1, 1, (u32)&buf[1]);
        r29 = 0;
        if (result == -1) {
            r30 = 1;
            continue;
        }
        if (result < 0 || result >= 7) {
            tblVal2 = 2;
        } else {
            r31 = *(u32*)(lbl_802677D0 + (u32)result * 0x10 + 8);
            tblVal2 = *(s32*)(lbl_802677D0 + (u32)result * 0x10 + 4);
        }
        if (tblVal2 == 0) {
            buf[0] = fn_80059034(r31);
            r29 = &buf[0];
        } else if (tblVal2 == 1) {
            r30 = 1;
        }
    } while (r30 == 0);

    fn_80102568(0x94, 2, 0);
    menuCloseSync(0x94, 1);
}
#pragma peephole on
#endif
