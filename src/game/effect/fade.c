/**
 * @file fade.c
 * @brief Screen fade state machine (retail game/pxdvs/app/fade/fade.cpp).
 *
 * Address range: 0x801C4078 - 0x801C4814 (11 functions)
 * Small data:    .sdata2 0x8047DFA8 - 0x8047DFD8 (the two fade colours,
 *                this TU's float literals and its int-to-float constants)
 *
 * The whole TU is built with -opt nopeephole (see configure.py).
 *
 * The fade state is shared between the fade daemon, which advances it once
 * per frame, and callers that poll it from other GS threads (fadeCheck spins
 * on the pending flag until the daemon clears it), so the object is declared
 * volatile: every read and write of it is a real memory access in retail.
 */

#include "dolphin/types.h"

typedef struct GStexture GStexture;

/* Per-frame fade hook installed by a hook's init function through
 * fadeSetFunctionOnly; returns 0 when the effect has finished. */
typedef u8 (*FadeHook)(u8 pending, f32 elapsed, f32 duration, f32 hookElapsed,
                       f32 hookDuration, GStexture* texture);

typedef struct FadeState {
    /* 0x00 */ u8 mode;         /* 0 idle, 1/2 fade out/in, 3/4 held */
    /* 0x01 */ u8 pending;      /* 1 while a timed fade is running */
    /* 0x02 */ u16 type;        /* bit 0: fade in, bit 1: black, bit 3: hold */
    /* 0x04 */ f32 duration;
    /* 0x08 */ f32 elapsed;
    /* 0x0C */ FadeHook hook;
    /* 0x10 */ GStexture* texture; /* back-buffer snapshot for the hook */
    /* 0x14 */ f32 hookDuration;
    /* 0x18 */ f32 hookElapsed;
} FadeState;

typedef struct FadeColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} FadeColor;

extern volatile FadeState lbl_80466E30;
extern u8 lbl_8047B3A8;

static const FadeColor sFadeWhite = { 0xFF, 0xFF, 0xFF, 0xFF };
static const FadeColor sFadeBlack = { 0x00, 0x00, 0x00, 0xFF };

extern void _threadSwitch(void);
extern void fadeEffectHookFunction_Doku_Init(void);
extern void soundStartSE(s32 arg0);
extern u8 menuOffScreenIsDoing(void);
extern GStexture* menuOffScreenGetPtr(void);
extern GStexture* GStextureCreate(u32 arg0, u32 arg1, u32 format, u32 arg3, u32 arg4);
extern void GStextureFree(GStexture* texture);
extern void GSgfxBeginBackFBCapture(GStexture* texture, void* callback, void* userData);
extern void fn_801C6928(void);
extern s32 fn_800D37CC(void);
extern u32 GSgetFrameStep(void);
extern void fn_800D9ED8(u32 arg0);
extern void gfxSetFlags(u32 arg0);
extern void gfxClearFlags(u32 arg0);
extern void gfxSetOrtho(f32 left, f32 top, f32 right, f32 bottom);
extern void gfxSetBlendMode(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetUpdates(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetZMode(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetCullMode(u32 arg0);
extern void gfxSetPrimitive(u32 arg0);
extern void gfxSetVtxDesc(u32 arg0);
extern void gfxBegin(u32 arg0);
extern void gfxVtxPos(f32 x, f32 y, f32 z);
extern void fn_800D5CB8(u32 arg0, u8 r, u8 g, u8 b, u8 a);
extern void gfxEnd(void);

FadeHook fadeSetEX(u16 type, void (*init)(void), u8 snapshot, f32 duration, f32 hookDuration);
void fadeSet(u16 type, f32 duration);
FadeHook fadeSetFunction__FPFv_vbUsf(void (*init)(void), u8 snapshot, u16 type, f32 hookDuration);
void _fadeSnapshot__Fv(void);
void* myBackFB__FP9GStextureUlPv(GStexture* texture, u32 size, void* userData);

/* 0x801C4078 | 0x24: cancel a held "doku" (poison) fade. */
void fadeEffectDokuStop(void) {
    if (lbl_80466E30.mode == 4) {
        lbl_80466E30.pending = 0;
        lbl_80466E30.mode = 0;
    }
}

/* 0x801C409C | 0x54: start the held "doku" (poison) fade when idle. */
void fadeEffectDokuStart(void) {
    if (lbl_80466E30.mode == 0) {
        fadeSetEX(9, fadeEffectHookFunction_Doku_Init, 0, 1.0f, 0.5f);
        soundStartSE(0x54);
    }
}

/* 0x801C40F0 | 0x74: return the pending flag, optionally waiting for the
 * running fade to finish first. */
s8 fadeCheck(u8 wait) {
    volatile FadeState* state;

    if (wait <= 0) {
        return lbl_80466E30.pending;
    }
    state = &lbl_80466E30;
    while (state->pending == 1) {
        _threadSwitch();
    }
    _threadSwitch();
    return lbl_80466E30.pending;
}

/* 0x801C4164 | 0x64: install a hook and start a timed fade. */
FadeHook fadeSetEX(u16 type, void (*init)(void), u8 snapshot, f32 duration, f32 hookDuration) {
    FadeHook previous;

    previous = fadeSetFunction__FPFv_vbUsf(init, snapshot, type, hookDuration);
    fadeSet(type, duration);
    return previous;
}

/* 0x801C41C8 | 0x74: start a timed fade. */
void fadeSet(u16 type, f32 duration) {
    lbl_80466E30.pending = 1;
    lbl_80466E30.type = type;
    lbl_80466E30.duration = duration;
    lbl_80466E30.elapsed = 0.0f;

    if (type & 8) {
        if (type & 1) {
            lbl_80466E30.mode = 4;
        } else {
            lbl_80466E30.mode = 3;
        }
    } else {
        if (type & 1) {
            lbl_80466E30.mode = 2;
        } else {
            lbl_80466E30.mode = 1;
        }
    }
}

/* 0x801C423C | 0xE0: reset the hook state, optionally snapshot the back
 * buffer, and run the hook's init function; returns the previous hook. */
FadeHook fadeSetFunction__FPFv_vbUsf(void (*init)(void), u8 snapshot, u16 type, f32 hookDuration) {
    FadeHook previous;

    if (init == NULL) {
        return lbl_80466E30.hook;
    }

    lbl_80466E30.pending = 1;
    lbl_80466E30.type = type;
    lbl_80466E30.hookDuration = hookDuration;
    lbl_80466E30.hookElapsed = 0.0f;
    lbl_80466E30.mode = 0;
    previous = lbl_80466E30.hook;
    lbl_80466E30.hook = NULL;

    if (snapshot == 1) {
        _fadeSnapshot__Fv();
    } else if (lbl_80466E30.texture != NULL) {
        if (lbl_80466E30.texture != menuOffScreenGetPtr()) {
            GStextureFree(lbl_80466E30.texture);
        }
        lbl_80466E30.texture = NULL;
    }

    fn_801C6928();
    init();
    return previous;
}

/* 0x801C431C | 0x10: install the per-frame hook (called by hook inits). */
void fadeSetFunctionOnly(FadeHook hook) {
    lbl_80466E30.hook = hook;
}

/* 0x801C432C | 0xB8: capture the back buffer into the fade texture. */
void _fadeSnapshot__Fv(void) {
    GStexture* texture;

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

/* 0x801C43E4 | 0x10: back-buffer capture completion callback. */
void* myBackFB__FP9GStextureUlPv(GStexture* texture, u32 size, void* userData) {
    lbl_8047B3A8 = 1;
    return NULL;
}

/* 0x801C43F4 | 0x3DC: per-frame fade update and full-screen quad draw. */
void fadeDaemon(void) {
    FadeColor white = sFadeWhite;
    FadeColor black = sFadeBlack;
    FadeColor color;
    f32 fps;
    f32 progress;
    u8 alpha;

    if (lbl_80466E30.pending == 1) {
        switch (lbl_80466E30.mode) {
        case 0:
        case 4:
            break;
        case 1:
        case 2:
        case 3:
            fps = fn_800D37CC();
            lbl_80466E30.elapsed += (f32)GSgetFrameStep() / fps;
            if (lbl_80466E30.elapsed >= lbl_80466E30.duration) {
                lbl_80466E30.elapsed = lbl_80466E30.duration;
                lbl_80466E30.pending = 0;
                if (lbl_80466E30.type & 1) {
                    lbl_80466E30.mode = 2;
                } else {
                    lbl_80466E30.mode = 0;
                }
            }
            break;
        }
    }

    if (lbl_80466E30.hook != NULL) {
        fps = fn_800D37CC();
        lbl_80466E30.hookElapsed += (f32)GSgetFrameStep() / fps;
        if (lbl_80466E30.hookElapsed >= lbl_80466E30.hookDuration) {
            lbl_80466E30.hookElapsed = lbl_80466E30.hookDuration;
        }
        if (!lbl_80466E30.hook(lbl_80466E30.pending, lbl_80466E30.elapsed, lbl_80466E30.duration,
                               lbl_80466E30.hookElapsed, lbl_80466E30.hookDuration,
                               lbl_80466E30.texture)) {
            lbl_80466E30.hook = NULL;
            if (lbl_80466E30.texture != NULL) {
                if (lbl_80466E30.texture != menuOffScreenGetPtr()) {
                    GStextureFree(lbl_80466E30.texture);
                }
                lbl_80466E30.texture = NULL;
            }
        }
    }

    switch (lbl_80466E30.mode) {
    case 0:
        break;
    case 1:
    case 2:
        if (lbl_80466E30.duration == 0.0f) {
            progress = 1.0f;
        } else {
            progress = lbl_80466E30.elapsed / lbl_80466E30.duration;
        }
        if (lbl_80466E30.type & 1) {
            alpha = 255.0f * progress;
        } else {
            alpha = 255 - (s32)(255.0f * progress);
        }
        if (lbl_80466E30.type & 2) {
            color = black;
        } else {
            color = white;
        }

        fn_800D9ED8(1);
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
        gfxSetBlendMode(1, 6, 7);
        gfxSetUpdates(1, 1, 0);
        gfxSetZMode(0, 1, 1);
        gfxSetCullMode(0);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(0);
        gfxBegin(2);
        gfxVtxPos(0.0f, 0.0f, 0.0f);
        fn_800D5CB8(0, color.r, color.g, color.b, alpha);
        gfxVtxPos(640.0f, 480.0f, 0.0f);
        fn_800D5CB8(0, color.r, color.g, color.b, alpha);
        gfxEnd();
        fn_800D9ED8(0);
        break;
    case 3:
    case 4:
        break;
    }
}

/* 0x801C47D0 | 0x44: reset the fade state. */
void fadeInit(void) {
    lbl_80466E30.mode = 0;
    lbl_80466E30.pending = 0;
    lbl_80466E30.type = 0;
    lbl_80466E30.duration = 0.0f;
    lbl_80466E30.elapsed = 0.0f;
    lbl_80466E30.hook = NULL;
    lbl_80466E30.texture = NULL;
    lbl_80466E30.hookDuration = 0.0f;
    lbl_80466E30.hookElapsed = 0.0f;
}
