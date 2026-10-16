/**
 * @file gs_gfx_range_800D2B90.c
 * @brief GS render/graphics TU span 0x800D2B90 - 0x800D377C with its
 *        .sdata2 literal pool 0x8047C9F0 - 0x8047CA10.
 *
 * One object for the fog setter fn_800D2B90, the camera screen projections
 * fn_800D2DE8/fn_800D2F34, the render-state accessors fn_800D305C/
 * fn_800D3068, the GSgfx frame functions fn_800D3074-fn_800D361C and the
 * fog-colour setter fn_800D36B4. The pool entries 255.0f, the unsigned
 * int-to-float bias, 0.0f, 640.0f, 480.0f and 1.0f are used only by these
 * functions, so they are written as literals and the object owns them.
 * This replaces the former per-function carves and their extern stand-ins
 * (lbl_8047C9F0, lbl_8047CA00-lbl_8047CA0C).
 *
 * Compiler: GC/2.5 with the project defaults (-O4,p), no pragmas.
 * fn_800D36B4 schedules its by-value colour loads ahead of its stack frame
 * and the enable-byte store. GC/2.5-2.7 emit that; GC/1.0-2.0 keep the
 * loads behind the frame and the store. Every other function here is
 * identical under GC/1.3 and GC/2.5.
 *
 * The render-state block at lbl_8047AA80 is reached through two partial
 * reconstructions: GSgfxState (game/gs_gfx.h) for the frame functions and
 * GSRenderState (game/gs_render_util.h, via RS) for the fog and accessors.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "game/gs_gfx.h"
#include "game/gs_render_util.h"
#include "game/gs_model.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_class.h"

typedef struct GSRenderColor {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
} GSRenderColor;

typedef f32 GSMtx34[3][4];
typedef f32 GSMtx44[4][4];

extern u32 lbl_8047AA8C; /* current fog */
extern GSRenderColor lbl_80270350;
extern void* HSD_FogLoadDesc(void* desc);
extern void HSD_FogSet(u32 fog);
extern void fn_8016EA88(void);
extern void fn_8016EB30(void);
extern void* gfxGetCameraMatrix(u32 index);
extern void GSvecTransform(f32* out, GSMtx34 m, const f32* in);
extern void GXProject(f32 x, f32 y, f32 z, GSMtx34 mtx, const f32* pm,
                      const f32* vp, f32* sx, f32* sy, f32* sz);
extern void gfxRecordCommand(s32 arg0, ...);
extern void GXFlush(void);
extern void GSgfxBackFBDoFrame(void);
extern void fn_801BF8A0(s32 value);
extern void fn_801E16F0(void);
extern void fn_801BF6AC(void);
extern void GStextureConvertFromHW(void* texture, s32 mode);
extern void fn_800B8E74(void);
extern void GXInvalidateTexAll(void);
extern u32 lbl_804001F0[]; /* frame statistics block */
extern u8 lbl_8047AA90;    /* profiling enabled */
extern u8 lbl_8047AA91;    /* GX draw done */
extern void GXSetDrawDone(void);
extern void OSYieldThread(void);
extern u32 OSGetTick(void);
extern void fn_8019C6FC(void);
extern void fn_800D1070(u32 frames);
extern void GSlightUpdate(u32 frames);
extern void fn_800E3884(u32 frames, s32 mode);
extern void fn_801181B0(s32 frames);
extern void fn_800D13C4(void* a);
extern void fn_800DC874(void* a);
extern void gsParticleUpdate(void* a);
extern void modelShadowRender__FP10GSgfxLayer(void);
extern void fn_8019C708(s32 a);
extern void HSD_SetEraseColor(u8 a, u8 b, u8 c, u8 d);
extern void HSD_EraseRect(s32 a, s32 b, s32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
extern void VIWaitForRetrace(void);

#define RS ((GSRenderState*)lbl_8047AA80)

/* fn_800D36B4's body, expanded into fn_800D2B90 with the colour passed by
 * value (retail copies it to a stack temporary first).
 * RULE-EXCEPTION(title-path): single-use inline helper duplicating
 * fn_800D36B4's body - see docs/RULE_EXCEPTIONS.md. fn_800D36B4 lies after
 * fn_800D2B90, so this non-deferred TU cannot inline the real function. */
static inline void gsSetFogColor(GSRenderColor color) {
    RS->fogEnabled = 1;
    RS->fogColorR = 255.0f * color.r;
    RS->fogColorG = 255.0f * color.g;
    RS->fogColorB = 255.0f * color.b;
    RS->fogColorA = 255.0f * color.a;
    if (RS->fogColorR == 0 && RS->fogColorG == 0 && RS->fogColorB == 0 &&
        RS->fogColorA == 0) {
        RS->fogEnabled = 0;
    }
}

void gfxSetFog(void* arg1) {
    GSRenderColor color;
    void* prevLight;
    color = lbl_80270350;
    prevLight = (void*)lbl_8047AA8C;
    if (prevLight != NULL) {
        if (prevLight != NULL && ref_DEC(prevLight)) {
            hsdDelete(prevLight);
        }
        lbl_8047AA8C = 0;
    }
    if (arg1 == 0) {
        HSD_FogSet(0);
        fn_8016EA88();
        return;
    }
    lbl_8047AA8C = (u32)HSD_FogLoadDesc(arg1);
    fn_8016EB30();
    color.r = *(u8*)((u8*)arg1 + 0x10) / 255.0f;
    color.g = *(u8*)((u8*)arg1 + 0x11) / 255.0f;
    color.b = *(u8*)((u8*)arg1 + 0x12) / 255.0f;
    color.a = *(u8*)((u8*)arg1 + 0x13) / 255.0f;
    gsSetFogColor(color);
}

/* 0x800D2DE8 | 0x14C */
s32 fn_800D2DE8(f32* points, f32* screen, u32 count)
{
    f32 projection[7];
    f32 viewport[6];
    f32 view[3];
    GSMtx44* proj;
    GSMtx34* viewMtx;
    u32 i;
    f32* src;
    f32* dst;
    s32 result;

    src = points;
    dst = screen;
    viewMtx = gfxGetCameraMatrix(0);
    proj = gfxGetCameraMatrix(2);
    if (viewMtx == NULL || proj == NULL) {
        return 0;
    }

    projection[0] = 0.0f;
    projection[1] = (*proj)[0][0];
    projection[2] = (*proj)[0][2];
    projection[3] = (*proj)[1][1];
    projection[4] = (*proj)[1][2];
    projection[5] = (*proj)[2][2];
    projection[6] = (*proj)[2][3];
    viewport[0] = 0.0f;
    viewport[1] = 0.0f;
    viewport[2] = 640.0f;
    viewport[3] = 480.0f;
    viewport[4] = 0.0f;
    viewport[5] = 1.0f;

    result = 2;
    for (i = 0; i < count; i++, src += 3, dst += 3) {
        GSvecTransform(view, *viewMtx, src);
        if (view[2] >= (*proj)[2][3]) {
            result = 3;
            dst[0] = dst[1] = dst[2] = 1.0f;
        } else {
            GXProject(src[0], src[1], src[2], *viewMtx, projection,
                      viewport, &dst[0], &dst[1], &dst[2]);
        }
    }
    return result;
}

/* 0x800D2F34 | 0x128 */
s32 gfxWorldToScreen(f32* point, f32* screen)
{
    f32 projection[7];
    f32 viewport[6];
    f32 view[3];
    GSMtx34* viewMtx;
    GSMtx44* proj;
    f32 far;

    viewMtx = gfxGetCameraMatrix(0);
    proj = gfxGetCameraMatrix(2);
    if (viewMtx == NULL || proj == NULL) {
        return 0;
    }

    GSvecTransform(view, *viewMtx, point);
    if (view[2] >= (far = (*proj)[2][3])) {
        return 1;
    }

    projection[0] = 0.0f;
    projection[1] = (*proj)[0][0];
    projection[2] = (*proj)[0][2];
    projection[3] = (*proj)[1][1];
    projection[4] = (*proj)[1][2];
    projection[5] = (*proj)[2][2];
    projection[6] = far;
    viewport[0] = 0.0f;
    viewport[1] = 0.0f;
    viewport[2] = 640.0f;
    viewport[3] = 480.0f;
    viewport[4] = 0.0f;
    viewport[5] = 1.0f;
    GXProject(point[0], point[1], point[2], *viewMtx, projection, viewport,
              &screen[0], &screen[1], &screen[2]);
    return 2;
}

void fn_800D305C(u8 level)
{
    RS->frameLevel = level;
}

u32 fn_800D3068(void)
{
    return RS->renderWidth;
}

void fn_800D3074(u32 flag)
{
    if (flag == 0) {
        return;
    }

    lbl_8047AA80->renderEnabled = flag;
}

u32 GSgetFrameStep(void)
{
    return lbl_8047AA80->frameDelta;
}

u32 fn_800D3094(void)
{
    return lbl_8047AA80->xfbCount;
}

void fn_800D30A0(u32 value)
{
    lbl_8047AA80->xfbIndex = value;
}

void fn_800D30AC(void)
{
    if (lbl_8047AA80->mode == 1) {
        gfxRecordCommand(4, 0);
    } else {
        GXFlush();
    }
}

void fn_800D30F0(u32 flag)
{
    void* renderTarget = lbl_8047AA80->renderTarget;

    if (renderTarget == (void*)0xFEFEFEFEU) {
        return;
    }

    if (lbl_8047AA80->progressiveFlag == 0) {
        GSgfxBackFBDoFrame();
        fn_801BF8A0(0);
        fn_801E16F0();
        fn_801BF6AC();
    } else if (renderTarget != NULL) {
        GStextureConvertFromHW(renderTarget, 1);
        flag = 0;
    }

    lbl_8047AA80->progressiveFlag = 1;

    if ((u8)flag != 0) {
        fn_800B8E74();
        if (lbl_8047AA80->renderTarget != NULL) {
            GXInvalidateTexAll();
        }
    }
}

void fn_800D3190(void)
{
    u32 start;
    u32 end;
    void* renderTarget;
    u8 flag;
    s32 count;

    if (lbl_8047AA80->renderTarget == (void*)0xFEFEFEFEU) {
        return;
    }

    lbl_8047AA91 = 0;
    GXFlush();
    GXSetDrawDone();
    start = OSGetTick();

    while (lbl_8047AA91 == 0) {
        OSYieldThread();
        if (lbl_8047AA91 == 0) {
            end = OSGetTick();
            if ((end - start) / (*(u32*)0x800000F8 / 4) > 3) {
                lbl_8047AA91 = 1;
            }
        }
    }

    fn_8019C6FC();

    renderTarget = lbl_8047AA80->renderTarget;
    if (renderTarget != NULL) {
        flag = 1;
        if (renderTarget != (void*)0xFEFEFEFEU) {
            if (lbl_8047AA80->progressiveFlag == 0) {
                GSgfxBackFBDoFrame();
                fn_801BF8A0(0);
                fn_801E16F0();
                fn_801BF6AC();
            } else if (renderTarget != NULL) {
                GStextureConvertFromHW(renderTarget, 1);
                flag = 0;
            }
            lbl_8047AA80->progressiveFlag = 1;
            if (flag != 0) {
                fn_800B8E74();
                if (lbl_8047AA80->renderTarget != NULL) {
                    GXInvalidateTexAll();
                }
            }
        }
    } else {
        lbl_804001F0[0] = lbl_804001F0[1];
        lbl_804001F0[2] = lbl_804001F0[3];
        lbl_804001F0[4] += 1;
        lbl_804001F0[1] = 0;
        lbl_804001F0[3] = 0;
    }

    lbl_8047AA80->renderTarget = (void*)0xFEFEFEFEU;

    if (lbl_8047AA90 == 0) {
        return;
    }

    start = OSGetTick();
    fn_800D1070(lbl_8047AA80->frameDelta);
    end = OSGetTick();
    lbl_804001F0[11] += end - start;

    start = OSGetTick();
    GSlightUpdate(lbl_8047AA80->frameDelta);
    end = OSGetTick();
    lbl_804001F0[12] += end - start;

    start = OSGetTick();
    fn_800E3884(lbl_8047AA80->frameDelta, 0);
    end = OSGetTick();
    lbl_804001F0[13] += end - start;

    count = lbl_8047AA80->frameDelta;
    lbl_804001F0[14] = 0;
    while (count-- != 0) {
        start = OSGetTick();
        fn_800E3884(1, 1);
        end = OSGetTick();
        lbl_804001F0[13] += end - start;

        start = OSGetTick();
        fn_801181B0(1);
        end = OSGetTick();
        lbl_804001F0[14] += end - start;
    }
}

void fn_800D3410(void* arg0, u8 arg1) {
    u32* state;
    u32 sc;
    u8 r30;
    u32 startTick;
    u32 tick;

    state = (u32*)lbl_8047AA80;
    sc = state[0xC / 4];

    if ((u32)(sc + 0x01020000U) == 0xFEFEU) {
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

        lbl_8047AA90 = arg1;

        if (arg1 != 0) {
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
                    0.0f,
                    480.0f,
                    0.0f,
                    640.0f,
                    0.0f);
            }

            ((u8*)lbl_8047AA80)[0x49D] = 0;
        } else {
            fn_8019C708(3);
        }
    }
}

void fn_800D361C(u8 mode) {
    VIWaitForRetrace();
    lbl_8047AA80->frameDelta = lbl_8047AA80->xfbCount - lbl_8047AA80->xfbAddr0;

    if (mode == 1) {
        while (lbl_8047AA80->frameDelta < lbl_8047AA80->renderEnabled) {
            VIWaitForRetrace();
            lbl_8047AA80->frameDelta = lbl_8047AA80->xfbCount - lbl_8047AA80->xfbAddr0;
        }

        if (lbl_8047AA80->vsyncFlag == 0) {
            lbl_8047AA80->frameDelta = lbl_8047AA80->renderEnabled;
        }
    }

    lbl_8047AA80->xfbAddr0 = lbl_8047AA80->xfbCount;
}

void fn_800D36B4(GSRenderColor color)
{
    RS->fogEnabled = 1;
    RS->fogColorR = 255.0f * color.r;
    RS->fogColorG = 255.0f * color.g;
    RS->fogColorB = 255.0f * color.b;
    RS->fogColorA = 255.0f * color.a;
    if (RS->fogColorR == 0 && RS->fogColorG == 0 && RS->fogColorB == 0 &&
        RS->fogColorA == 0) {
        RS->fogEnabled = 0;
    }
}
