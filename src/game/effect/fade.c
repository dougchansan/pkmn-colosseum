/**
 * @file fade.c
 * @brief Battle grid fade/transition state machine and battle camera
 *        angle/sequence control.
 *
 * Address range: 0x801C4078 - 0x801C4814 (11 functions).
 *
 * Split out of the former monolithic battle_grid.c CodeCandidate bucket
 * (0x801C0F20 - 0x801C4CB8, split pass 2026-07-07). This is a distinct
 * XD translation unit (game/pxdvs/app/fade/fade.cpp).
 */

#include "dolphin/types.h"
#include "game/battle/battle_grid_types.h"

typedef struct BattleGridTransitionState {
    u8 mode;
    u8 pending;
    u16 arg;
    f32 startValue;
    f32 endValue;
    void* callbackArg;
    void* texture;
    f32 value;
    f32 timer;
} BattleGridTransitionState;

typedef struct BattleGridCameraWork {
    u8 pad_00[4];
    f32 angle;
    f32 blend;
    s32 sequenceType;
    f32 sequenceParam1;
    f32 sequenceParam2;
    f32 sequenceTimer;
} BattleGridCameraWork;

void* fadeSetEX(s32 mode, void* callback, s32 flags, f32 a, f32 b);
void fadeSet(s32 mode, f32 value);
void* fadeSetFunction__FPFv_vbUsf(void (*callback)(void), s32 mode, u32 arg,
                                  f32 value);

/**
 * fadeEffectDokuStop - Get grid slot model pointer (renamed
 * from fn_801C4078; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C4078 | Size: 0x24
 */
#pragma peephole off
void* fadeEffectDokuStop(s32 slot) {
    extern BattleGridTransitionState lbl_80466E30;
    volatile BattleGridTransitionState* gridState = &lbl_80466E30;

    if (gridState->mode == 4) {
        gridState->pending = 0;
        gridState->mode = 0;
    }

    return (void*)gridState;
}
#pragma peephole on

/**
 * fadeEffectDokuStart - Trigger grid slot update callback.
 * Address: 0x801C409C | Size: 0x54
 */
#pragma peephole off
void fadeEffectDokuStart(void) {
    extern BattleGridTransitionState lbl_80466E30;
    extern const f32 lbl_8047DFB0;
    extern const f32 lbl_8047DFB4;
    extern void fadeEffectHookFunction_Doku_Init(s32 slot); /* game/effect/fade_effect.c, renamed from fn_801C4814 */
    extern void fn_80166A28(s32 arg0);

    if (lbl_80466E30.mode == 0) {
        fadeSetEX(9, (void*)fadeEffectHookFunction_Doku_Init, 0, lbl_8047DFB0, lbl_8047DFB4);
        fn_80166A28(0x54);
    }
}
#pragma peephole on

/**
 * fadeCheck - Set grid rendering flag.
 * Address: 0x801C40F0 | Size: 0x74
 * Referenced by battle_main.c as "battle grid set flag".
 */
#pragma peephole off
s32 fadeCheck(u8 flag) {
    extern u8 lbl_80466E30[];
    u8* gridState;

    if (flag <= 0) {
        return (s8)lbl_80466E30[1];
    }

    gridState = lbl_80466E30;
    while (gridState[1] == 1) {
        _threadSwitch();
    }
    _threadSwitch();
    return (s8)lbl_80466E30[1];
}
#pragma peephole on

/**
 * fadeSetEX - Schedule grid update callback with arguments.
 * Address: 0x801C4164 | Size: 0x64
 */
void* fadeSetEX(s32 mode, void* callback, s32 flags, f32 a, f32 b) {
    void* previous;

    previous = fadeSetFunction__FPFv_vbUsf((void (*)(void))callback, flags,
                                           mode, b);
    fadeSet(mode, a);
    return previous;
}

/**
 * fadeSet - Battle camera initialization.
 * Address: 0x801C41C8 | Size: 0x74
 * Referenced by battle_main.c as "battle camera init".
 * Initializes the battle camera to the default overhead view
 * and configures the camera animation system.
 */
void fadeSet(s32 mode, f32 value) {
    extern BattleGridTransitionState lbl_80466E30;
    extern const f32 lbl_8047DFB8;
    u16 modeValue = mode;

    lbl_80466E30.pending = 1;
    lbl_80466E30.arg = modeValue;
    lbl_80466E30.startValue = value;
    lbl_80466E30.endValue = lbl_8047DFB8;

    if (modeValue & 8) {
        if (modeValue & 1) {
            lbl_80466E30.mode = 4;
        } else {
            lbl_80466E30.mode = 3;
        }
    } else {
        if (modeValue & 1) {
            lbl_80466E30.mode = 2;
        } else {
            lbl_80466E30.mode = 1;
        }
    }
}

/**
 * fadeSetFunction__FPFv_vbUsf - Battle grid callback/state transition
 * (renamed from fn_801C423C; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C423C | Size: 0xE0
 */
#pragma peephole off
void* fadeSetFunction__FPFv_vbUsf(void (*callback)(void), s32 mode, u32 arg, f32 value) {
    extern u8 lbl_80466E30[];
    extern volatile const f32 lbl_8047DFB8;
    extern u32 menuOffScreenGetPtr(void);
    extern void GStextureFree(void* texture);
    extern void _fadeSnapshot__Fv(); /* renamed from fn_801C432C */
    extern void fn_801C6928(void);
    void* previous;
    long new_var;
    void (*savedCallback)(void);
    u8* gridState;
    u32 modeByte;
    u32 argHalf;

    savedCallback = callback;
    if (callback == NULL) {
        return *(void**)(lbl_80466E30 + 0xC);
    }

    modeByte = (u8)mode;
    argHalf = arg & 0xFFFF;
    gridState = lbl_80466E30;
    *(volatile u8*)(gridState + 1) = 1;
    *(volatile u16*)(gridState + 2) = argHalf;
    *(volatile f32*)(gridState + 0x14) = value;
    *(volatile f32*)(gridState + 0x18) = lbl_8047DFB8;
    *(volatile u8*)gridState = 0;
    previous = *(void* volatile *)(gridState + 0xC);
    *(void* volatile *)(0xC + gridState) = NULL;

    if (modeByte == 1) {
        _fadeSnapshot__Fv();
    } else if (*(void* volatile *)(gridState + 0x10) != (0, NULL)) {
        if (*(u32 volatile *)(lbl_80466E30 + 0x10) != menuOffScreenGetPtr()) {
            GStextureFree(*(void* volatile *)(lbl_80466E30 + 0x10));
        }
        new_var = 0x10;
        *(void* volatile *)(lbl_80466E30 + new_var) = NULL;
    }

    fn_801C6928();
    savedCallback();
    return previous;
}
#pragma peephole on

/**
 * fadeSetFunctionOnly - Get camera current angle (renamed from
 * fn_801C431C; confirmed name -- naming pass 2026-07-07). Called
 * cross-TU by game/effect/fade_effect.c's fadeEffectHookFunction_* family.
 * Address: 0x801C431C | Size: 0x10
 */
void fadeSetFunctionOnly(s32 arg0) {
    extern BattleGridTransitionState lbl_80466E30;

    lbl_80466E30.callbackArg = (void*)arg0;
}

/**
 * _fadeSnapshot__Fv - Camera angle calculation (renamed from fn_801C432C;
 * confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C432C | Size: 0xB8
 */
void _fadeSnapshot__Fv(void) {
    extern BattleGridTransitionState lbl_80466E30;
    extern u8 lbl_8047B3A8;
    extern u32 menuOffScreenIsDoing(void);
    extern void* menuOffScreenGetPtr(void);
    extern void* GStextureCreate(u32, u32, u32, u32, u32);
    extern void GSgfxBeginBackFBCapture(void*, void*, void*);
    extern void* myBackFB__FP9GStextureUlPv(void*, u32, void*);
    void* texture;

    texture = lbl_80466E30.texture;
    lbl_80466E30.texture = NULL;
    if (!menuOffScreenIsDoing()) {
        texture = menuOffScreenGetPtr();
    }
    if (texture == NULL) {
        texture = GStextureCreate(0, 0, 0x44, 0, 0);
    }
    if (texture != NULL) {
        lbl_8047B3A8 = 0;
        GSgfxBeginBackFBCapture(texture, myBackFB__FP9GStextureUlPv, NULL);
        while (lbl_8047B3A8 == 0) {
            _threadSwitch();
        }
        lbl_80466E30.texture = texture;
    }
}

/**
 * myBackFB__FP9GStextureUlPv - Get camera target position (renamed from
 * fn_801C43E4; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C43E4 | Size: 0x10
 */
void* myBackFB__FP9GStextureUlPv(void) {
    extern u8 lbl_8047B3A8;
    lbl_8047B3A8 = 1;
    return NULL;
}

/**
 * fadeDaemon - Camera complex movement sequence (renamed from
 * fn_801C43F4; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C43F4 | Size: 0x3DC
 */
void fadeDaemon(s32 seqType, f32 param1, f32 param2) {
    extern u8 lbl_80466E30[];
    extern u32 lbl_8047DFA8;
    extern u32 lbl_8047DFAC;
    extern f32 lbl_8047DFB8;
    extern f32 lbl_8047DFBC;
    extern f32 lbl_8047DFC0;
    extern f32 lbl_8047DFC4;
    extern f64 lbl_8047DFC8;
    extern f64 lbl_8047DFD0;
    extern s32 menuOffScreenGetPtr(void);
    extern void GStextureFree(void* texture);
    extern s32 fn_800D37CC(void);
    extern s32 fn_800D3088(void);
    extern void fn_800D9ED8(u32 arg0);
    extern void fn_800D88DC(u32 arg0);
    extern void fn_800D888C(u32 arg0);
    extern void fn_800D9B58(f32 left, f32 top, f32 right, f32 bottom);
    extern void fn_800DA4C4(u32 arg0, u32 arg1, u32 arg2);
    extern void fn_800DA2BC(u32 arg0, u32 arg1, u32 arg2);
    extern void fn_800DA1E8(u32 arg0, u32 arg1, u32 arg2);
    extern void fn_800DA028(u32 arg0);
    extern void fn_800D6A00(u32 arg0);
    extern void fn_800D7820(u32 arg0);
    extern void fn_800D67BC(u32 arg0);
    extern void fn_800D6680(f32 x, f32 y, f32 z);
    extern void fn_800D5CB8(u32 arg0, u32 r, u32 g, u32 b, u32 a);
    extern void fn_800D6728(void);

    typedef u8 (*FadeHook)(u8, void*, f32, f32, f32, f32);
    u8* state;
    FadeHook hook;
    void* texture;
    f32 fps;
    f32 step;
    f32 progress;
    u32 colorWord;
    u32 alpha;
    u32 r;
    u32 g;
    u32 b;

    (void)seqType;
    (void)param1;
    (void)param2;

    state = lbl_80466E30;

    if (state[1] == 1 && state[0] != 0 && state[0] < 4 && state[0] != 4) {
        fps = (f32)((f64)(s32)fn_800D37CC() - lbl_8047DFC8);
        step = (f32)((f64)(s32)fn_800D3088() - lbl_8047DFD0) / fps;
        *(f32*)(state + 0x08) += step;
        if (*(f32*)(state + 0x08) >= *(f32*)(state + 0x04)) {
            *(f32*)(state + 0x08) = *(f32*)(state + 0x04);
            state[1] = 0;
            if ((*(u16*)(state + 0x02) & 1) != 0) {
                state[0] = 2;
            } else {
                state[0] = 0;
            }
        }
    }

    hook = *(FadeHook*)(state + 0x0C);
    if (hook != NULL) {
        fps = (f32)((f64)(s32)fn_800D37CC() - lbl_8047DFC8);
        step = (f32)((f64)(s32)fn_800D3088() - lbl_8047DFD0) / fps;
        *(f32*)(state + 0x18) += step;
        if (*(f32*)(state + 0x18) >= *(f32*)(state + 0x14)) {
            *(f32*)(state + 0x18) = *(f32*)(state + 0x14);
        }

        texture = *(void**)(state + 0x10);
        if (hook(state[1], texture, *(f32*)(state + 0x08), *(f32*)(state + 0x04),
                 *(f32*)(state + 0x18), *(f32*)(state + 0x14)) == 0) {
            *(FadeHook*)(state + 0x0C) = NULL;
            if (texture != NULL) {
                if ((u32)texture != (u32)menuOffScreenGetPtr()) {
                    GStextureFree(texture);
                }
                *(void**)(state + 0x10) = NULL;
            }
        }
    }

    if (state[0] != 1 && state[0] != 2) {
        return;
    }

    if (*(f32*)(state + 0x04) == lbl_8047DFB8) {
        progress = 1.0f;
    } else {
        progress = *(f32*)(state + 0x08) / *(f32*)(state + 0x04);
    }

    if ((*(u16*)(state + 0x02) & 1) != 0) {
        alpha = (u32)(lbl_8047DFBC * progress);
    } else {
        alpha = 0xFF - (u32)(lbl_8047DFBC * progress);
    }

    if ((*(u16*)(state + 0x02) & 2) != 0) {
        colorWord = lbl_8047DFAC;
    } else {
        colorWord = lbl_8047DFA8;
    }
    r = (colorWord >> 24) & 0xFF;
    g = (colorWord >> 16) & 0xFF;
    b = (colorWord >> 8) & 0xFF;

    fn_800D9ED8(1);
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D9B58(lbl_8047DFB8, lbl_8047DFB8, lbl_8047DFC0, lbl_8047DFC4);
    fn_800DA4C4(1, 6, 7);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA028(0);
    fn_800D6A00(7);
    fn_800D7820(0);
    fn_800D67BC(2);
    fn_800D6680(lbl_8047DFB8, lbl_8047DFB8, lbl_8047DFB8);
    fn_800D5CB8(0, r, g, b, alpha & 0xFF);
    fn_800D6680(lbl_8047DFC0, lbl_8047DFC4, lbl_8047DFB8);
    fn_800D5CB8(0, r, g, b, alpha & 0xFF);
    fn_800D6728();
    fn_800D9ED8(0);
}

/**
 * fadeInit - Camera get current mode.
 * Address: 0x801C47D0 | Size: 0x44
 */
#pragma peephole off
s32 fadeInit(void) {
    extern BattleGridTransitionState lbl_80466E30;
    extern volatile const f32 lbl_8047DFB8;
    volatile BattleGridTransitionState* base = &lbl_80466E30;

    base->mode = 0;
    base->pending = 0;
    base->arg = 0;
    base->startValue = lbl_8047DFB8;
    base->endValue = lbl_8047DFB8;
    base->callbackArg = NULL;
    base->texture = NULL;
    base->value = lbl_8047DFB8;
    base->timer = lbl_8047DFB8;
    return (s32)base;
}
#pragma peephole on
