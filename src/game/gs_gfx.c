/**
 * @file gs_gfx.c
 * @brief GSgfx -- Genius Sonority graphics subsystem.
 *
 * Only the fn_-named leaf functions below (fn_800D3094, fn_800D30A0,
 * fn_800D30AC, fn_800D3190, fn_800D3410, fn_800D36B4, fn_800D377C) are
 * real members of this unit; several are already byte-matched.
 *
 * This file previously also carried a second, parallel implementation of
 * the unit under invented friendly names (GSgfxInit, GSgfxSetVideoMode,
 * GSgfxEnableRendering, GSgfxSwapBuffers, GSgfxSetDrawMode,
 * GSgfxGetFrameCount, GSgfxGetTickCount), guarded by `#ifdef PCPORT`.
 * PCPORT is never defined anywhere in this build (no such target exists
 * in configure.py), so that whole block was dead code that could never
 * be compiled into any real target. None of the seven names appear in
 * config/GC6E01/symbols.txt, and their claimed sizes didn't consistently
 * match the real fn_ symbols living at those addresses (e.g. fn_800D3088
 * is really 0xC bytes, not the claimed 0x68; fn_800D30F0 is really 0xA0,
 * not the claimed 0xF0). It was fiction from the old campaign transplant
 * and has been deleted wholesale.
 *
 * Address range: 0x800D3074 - 0x800D3E4C (approx.)
 */

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "game/gs_gfx.h"
#include "game/gs_model.h"

/* ===== Data referenced by the real fn_ functions below ===== */
extern u8 lbl_804001F0[];  /* GX state / FIFO state block */

#ifndef PCPORT
void fn_800D3074(u32 flag) {
    if (flag == 0) {
        return;
    }

    ((GSgfxState*)lbl_8047AA80)->renderEnabled = flag;
}

u32 GSgetFrameStep(void) {
    return lbl_8047AA80->frameDelta;
}

u32 fn_800D3094(void) {
    return *(u32*)((u8*)lbl_8047AA80 + 0x4C);
}
void fn_800D30A0(u32 val) {
    *(u32*)((u8*)lbl_8047AA80 + 0x48) = val;
}
extern void gfxRecordCommand(s32 arg0, ...);
extern void GXFlush(void);
#if 0
asm void fn_800D30AC(void) {
#include "src/game/gs_gfx_fn_800D30AC.inc"
}
#else
void fn_800D30AC(void) {
    u8* state = (u8*)lbl_8047AA80;

    if (*(s32*)(state + 0x0) == 1) {
        gfxRecordCommand(4, 0);
    } else {
        GXFlush();
    }
}
#endif
extern void GXSetDrawDone(void);
extern void OSYieldThread(void);
extern void fn_8019C6FC(void);
extern void GSgfxBackFBDoFrame(void);
extern void fn_801BF8A0(s32 a);
extern void fn_801E16F0(void);
extern void fn_801BF6AC(void);
extern void GStextureConvertFromHW(void* a, s32 b);
extern void fn_800B8E74(void);
extern void GXInvalidateTexAll(void);
extern void fn_800D1070(void* a);
extern void GSlightUpdate(void* a);
extern void fn_800E3884(void* a, s32 b);
extern void fn_801181B0(s32 a);
extern u32 OSGetTick(void);
extern u8 lbl_8047AA91;
extern u8 lbl_8047AA90;

void fn_800D30F0(u32 flag)
{
    u32 sc = (u32)lbl_8047AA80->renderTarget;

    if ((u32)(sc + 0x01020000U) == 0xFEFEU) {
        return;
    }

    if (lbl_8047AA80->progressiveFlag == 0) {
        GSgfxBackFBDoFrame();
        fn_801BF8A0(0);
        fn_801E16F0();
        fn_801BF6AC();
    } else {
        if (sc != 0) {
            GStextureConvertFromHW((void*)sc, 1);
            flag = 0;
        }
    }

    lbl_8047AA80->progressiveFlag = 1;

    if ((u8)flag != 0) {
        fn_800B8E74();
        if (lbl_8047AA80->renderTarget != NULL) {
            GXInvalidateTexAll();
        }
    }
}

#if 0
asm void fn_800D3190(void) {
#include "src/game/gs_gfx_fn_800D3190.inc"
}
#else
void fn_800D3190(void) {
    u32* state;
    u32 sc;
    u32 startTick;
    u32* r31ptr;
    u8 r30;
    u32 tick;
    u32 div;
    s32 r29count;
    u8 r29b;
    u32* s4;
    u32 chk;

    state = (u32*)lbl_8047AA80;
    sc = state[0xC / 4];
    if ((u32)(sc + 0x01020000U) == 0xFEFEU) {
        return;
    }

    lbl_8047AA91 = 0;
    GXFlush();
    GXSetDrawDone();
    startTick = OSGetTick();
    r31ptr = (u32*)0x80000000;
    r30 = 1;

    while (lbl_8047AA91 == 0) {
        OSYieldThread();
        if (lbl_8047AA91 == 0) {
            tick = OSGetTick();
            div = (tick - startTick) / (r31ptr[0xF8 / 4] >> 2);
            if (div > 3) {
                lbl_8047AA91 = r30;
            }
        }
    }

    fn_8019C6FC();

    s4 = (u32*)lbl_8047AA80;
    sc = s4[0xC / 4];
    if (sc != 0) {
        chk = sc + 0x01020000U;
        r29b = 1;
        if (chk != 0xFEFEU) {
            if (((u8*)s4)[0x49D] == 0) {
                GSgfxBackFBDoFrame();
                fn_801BF8A0(0);
                fn_801E16F0();
                fn_801BF6AC();
            } else {
                if (sc != 0) {
                    GStextureConvertFromHW((void*)sc, 1);
                    r29b = 0;
                }
            }
            ((u8*)lbl_8047AA80)[0x49D] = 1;
            if (r29b != 0) {
                fn_800B8E74();
                if (*(u32*)((u8*)lbl_8047AA80 + 0xC) != 0) {
                    GXInvalidateTexAll();
                }
            }
        }
    } else {
        u32* p = (u32*)&lbl_804001F0;
        p[0x0 / 4] = p[0x4 / 4];
        p[0x8 / 4] = p[0xC / 4];
        p[0x10 / 4] += 1;
        p[0x4 / 4] = 0;
        p[0xC / 4] = 0;
    }

    *(u32*)((u8*)lbl_8047AA80 + 0xC) = 0xFEFEFEFEU;

    if (lbl_8047AA90 == 0) {
        return;
    }

    startTick = OSGetTick();
    fn_800D1070((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
    tick = OSGetTick();
    ((u32*)&lbl_804001F0)[0x2C / 4] += tick - startTick;

    startTick = OSGetTick();
    GSlightUpdate((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
    tick = OSGetTick();
    ((u32*)&lbl_804001F0)[0x30 / 4] += tick - startTick;

    startTick = OSGetTick();
    fn_800E3884((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54), 0);
    tick = OSGetTick();
    ((u32*)&lbl_804001F0)[0x34 / 4] += tick - startTick;

    r29count = *(s32*)((u8*)lbl_8047AA80 + 0x54);
    ((u32*)&lbl_804001F0)[0x38 / 4] = 0;

    while (r29count-- != 0) {
        startTick = OSGetTick();
        fn_800E3884((void*)1, 1);
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x34 / 4] += tick - startTick;

        startTick = OSGetTick();
        fn_801181B0(1);
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x38 / 4] += tick - startTick;
    }
}
#endif
extern void fn_800D13C4(void* a);
extern void fn_800DC874(void* a);
extern void gsParticleUpdate(void* a);
extern void modelShadowRender__FP10GSgfxLayer(void);
extern void fn_8019C708(s32 a);
extern void HSD_SetEraseColor(u8 a, u8 b, u8 c, u8 d);
extern void HSD_EraseRect(s32 a, s32 b, s32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
extern u8 lbl_8047AA90;
extern f32 lbl_8047CA00;
extern f32 lbl_8047CA08;
extern f32 lbl_8047CA04;
#if 0
asm void fn_800D3410(void) {
#include "src/game/gs_gfx_fn_800D3410.inc"
}
#else
void fn_800D3410(void* arg0, u8 arg1) {
    u32* state;
    u32 sc;
    u8 r30;
    u32 startTick;
    u32 tick;

    state = (u32*)lbl_8047AA80;
    sc = state[0xC / 4];

    if ((u32)(sc + 0x01020000U) == 0xFEFEU) {
        /* sentinel matched — store arg0 into state->0xC and process sub-block */
        state[0xC / 4] = (u32)arg0;
        r30 = 0;

        {
            u32* s4 = (u32*)lbl_8047AA80;
            sc = s4[0xC / 4];
            if ((u32)(sc + 0x01020000U) != 0xFEFEU) {
                if (((u8*)s4)[0x49D] == 0) {
                    GSgfxBackFBDoFrame();
                    fn_801BF8A0(0);
                    fn_801E16F0();
                    fn_801BF6AC();
                } else {
                    if (sc != 0) {
                        GStextureConvertFromHW((void*)sc, 1);
                        r30 = 0;
                    }
                }
                ((u8*)lbl_8047AA80)[0x49D] = 1;
                if (r30 != 0) {
                    fn_800B8E74();
                    if (*(u32*)((u8*)lbl_8047AA80 + 0xC) != 0) {
                        GXInvalidateTexAll();
                    }
                }
            }
        }

    /* store arg1 into lbl_8047AA90 (SDA21 var) */
    lbl_8047AA90 = arg1;

    if (arg1 != 0) {
        /* reload state sub ptr and timing base inline each call (matches target) */
        startTick = OSGetTick();
        fn_800D13C4((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x2C / 4] = tick - startTick;

        startTick = OSGetTick();
        fn_800DC874((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x30 / 4] = tick - startTick;

        startTick = OSGetTick();
        fn_800E3928((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x34 / 4] = tick - startTick;

        startTick = OSGetTick();
        gsParticleUpdate((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x38 / 4] = tick - startTick;
    }

    if (arg0 == 0) {
        startTick = OSGetTick();
        modelShadowRender__FP10GSgfxLayer();
        tick = OSGetTick();
        ((u32*)&lbl_804001F0)[0x3C / 4] = tick - startTick;

        fn_8019C708(0);

        if (((u8*)lbl_8047AA80)[0x19] != 0) {
            HSD_SetEraseColor(
                ((u8*)lbl_8047AA80)[0x1C],
                ((u8*)lbl_8047AA80)[0x1D],
                ((u8*)lbl_8047AA80)[0x1E],
                ((u8*)lbl_8047AA80)[0x1F]
            );
            HSD_EraseRect(1, 1, 0,
                lbl_8047CA00,
                lbl_8047CA08,
                lbl_8047CA00,
                lbl_8047CA04,
                lbl_8047CA00);
        }

        ((u8*)lbl_8047AA80)[0x49D] = 0;
    } else {
        fn_8019C708(3);
    }
    }
}
#endif

extern void VIWaitForRetrace(void);
void fn_800D361C(u8 mode) {
    u32 diff;

    VIWaitForRetrace();
    diff = ((GSgfxState*)lbl_8047AA80)->xfbCount -
           ((GSgfxState*)lbl_8047AA80)->xfbAddr0;
    ((GSgfxState*)lbl_8047AA80)->frameDelta = diff;

    if ((mode == 1) != 0) {
        while (((GSgfxState*)lbl_8047AA80)->frameDelta <
               ((GSgfxState*)lbl_8047AA80)->renderEnabled) {
            VIWaitForRetrace();
            diff = ((GSgfxState*)lbl_8047AA80)->xfbCount -
                   ((GSgfxState*)lbl_8047AA80)->xfbAddr0;
            ((GSgfxState*)lbl_8047AA80)->frameDelta = diff;
        }

        if (((GSgfxState*)lbl_8047AA80)->vsyncFlag == 0) {
            ((GSgfxState*)lbl_8047AA80)->frameDelta =
                ((GSgfxState*)lbl_8047AA80)->renderEnabled;
        }
    }

    ((GSgfxState*)lbl_8047AA80)->xfbAddr0 =
        ((GSgfxState*)lbl_8047AA80)->xfbCount;
}

/* fn_800D36B4 (fog colour setter) is linked from gs_gfx_range_800D2B90.c. */
extern void fn_8019C690();
void fn_800D377C(s32 mode) {
    switch (mode) {
    case 1:
        fn_8019C690(0, 0);
        break;
    case 2:
        fn_8019C690(1, 0);
        break;
    }
}

extern s32 GSgfxVideoVsyncRate;
s32 fn_800D37CC(void) {
    return GSgfxVideoVsyncRate;
}

/* fn_800D37D4 and GSgfxInit, with the render-mode tables they own. */
#include "src/game/gs_gfx_candidate_800D37D4.c"
#endif /* !PCPORT */
