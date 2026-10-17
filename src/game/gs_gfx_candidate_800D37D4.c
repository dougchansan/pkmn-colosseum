/**
 * @file gs_gfx_candidate_800D37D4.c
 * @brief GSgfx video-mode selection and GSgfxInit, 0x800D37D4 - 0x800D3E4C.
 *
 * Part of the GSgfx translation unit.  Its .sbss (0x8047AA80 - 0x8047AAA8)
 * is shared by GSgfxRenderEnd ... fn_800D3F50, and its .data starts with the
 * render-mode tables below (0x803140F8 - 0x80314188).
 *
 * The four per-TV-standard tables are file-scope statics.  MWCC pools like
 * data objects, so fn_800D37D4 addresses them through one base register
 * loaded at entry (lis/addi of the pool) and indexes each table with
 * `addi base, offset` + lwzx; GC/1.3.2 keeps the per-table offset on the
 * base register, GC/1.3 folds it into the load displacement instead.
 *
 * Retail .rodata strings, .sdata2 constants and the .sbss state stay extern.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "game/gs_gfx.h"

/* SDK render modes (GX .data); name after the SDK object in the comment. */
extern GXRenderModeObj lbl_80312C40; /* GXNtsc240Ds */
extern GXRenderModeObj lbl_80312C7C; /* GXNtsc240DsAa */
extern GXRenderModeObj lbl_80312CB8; /* GXNtsc240Int */
extern GXRenderModeObj lbl_80312CF4; /* GXNtsc240IntAa */
extern GXRenderModeObj lbl_80312D30; /* GXNtsc480IntDf */
extern GXRenderModeObj lbl_80312D6C; /* GXNtsc480Int */
extern GXRenderModeObj lbl_80312DA8; /* GXNtsc480IntAa */
extern GXRenderModeObj lbl_80312DE4; /* GXNtsc480Prog */
extern GXRenderModeObj lbl_80312E20; /* GXNtsc480ProgAa */
extern GXRenderModeObj lbl_80312E5C; /* GXMpal240Ds */
extern GXRenderModeObj lbl_80312E98; /* GXMpal240DsAa */
extern GXRenderModeObj lbl_80312ED4; /* GXMpal240Int */
extern GXRenderModeObj lbl_80312F10; /* GXMpal240IntAa */
extern GXRenderModeObj lbl_80312F4C; /* GXMpal480IntDf */
extern GXRenderModeObj lbl_80312F88; /* GXMpal480Int */
extern GXRenderModeObj lbl_80312FC4; /* GXMpal480IntAa */
extern GXRenderModeObj lbl_80313000; /* GXPal264Ds */
extern GXRenderModeObj lbl_8031303C; /* GXPal264DsAa */
extern GXRenderModeObj lbl_80313078; /* GXPal264Int */
extern GXRenderModeObj lbl_803130B4; /* GXPal264IntAa */
extern GXRenderModeObj lbl_803130F0; /* GXPal528IntDf */
extern GXRenderModeObj lbl_8031312C; /* GXPal528Int */
extern GXRenderModeObj lbl_80313168; /* GXPal524IntAa */
extern GXRenderModeObj lbl_803131A4; /* GXEurgb60Hz240Ds */
extern GXRenderModeObj lbl_803131E0; /* GXEurgb60Hz240DsAa */
extern GXRenderModeObj lbl_8031321C; /* GXEurgb60Hz240Int */
extern GXRenderModeObj lbl_80313258; /* GXEurgb60Hz240IntAa */
extern GXRenderModeObj lbl_80313294; /* GXEurgb60Hz480IntDf */
extern GXRenderModeObj lbl_803132D0; /* GXEurgb60Hz480Int */
extern GXRenderModeObj lbl_8031330C; /* GXEurgb60Hz480IntAa */

extern s32 GSgfxVideoVsyncRate;
extern s32 lbl_8047AA98;  /* current TV standard (fn_800D37D4 mode) */
extern u8 lbl_8047AA9C;   /* 1 when a 50Hz-capable standard was chosen at init */
extern u32 lbl_8047AAA0;  /* EFB height override (0 = keep the table's) */
extern u8 lbl_8047AA84;
extern u8 lbl_8047AA85;
extern u32 lbl_8047AA8C;
extern u8 lbl_804001F0[]; /* per-frame timing counters (0x58 bytes) */
extern GXRenderModeObj lbl_80466BC0; /* HSD VI state, current.vi.rmode (first member) */
extern char lbl_80270360[]; /* "GSgfx: unable to allocate gsgfx state!\n" */
extern char lbl_80270388[]; /* "GSgfx: Init OK, state located at %08Xh (size=%d)\n" */
extern f32 lbl_8047CA10;  /* 30.0f */
extern f32 lbl_8047CA14;  /* 1.179f */
extern f32 lbl_8047CA18;  /* 0.1f */
extern f32 lbl_8047CA1C;  /* 30000.0f */

extern void* memcpy(void*, const void*, u32);
extern void VIWaitForRetrace(void);
extern void HSD_VISetBlack(u32 black);            /* HSD video: set black */
extern void HSD_VISetConfigure(GXRenderModeObj* rmode); /* HSD video: configure */
extern void GSmathInit(void);
extern void _gfxScratchNotify__F15GSscratchNotifyPvUc(void);
extern void* GSscratchAlloc(u32, void*);
extern u16 _toolentryAlloc__FUl(u32);
extern void* GSmemLock(u16);
extern void GSlogWrite(const char*, ...);
extern void HSD_SetInitParameter(s32, ...);
extern void HSD_InitComponent(void);
extern void fn_8019C690(u32, u32);
extern void HSD_VISetUserPostRetraceCallback(void*);
extern void HSD_VISetUserGXDrawDoneCallback(void*);
extern void fn_80196C3C(void*);
extern void fn_800D3F5C(void);
extern void fn_800D3F50(void);
extern void GSgfxSetViewportJitter(void);
extern void gfxAllocRequestBuffer(u32);
extern void gfxMatrixStackInit(u32);
extern void gfxVtxDescPoolInit(u32);
extern void fn_800DB890(u32);
extern u32 gfxVtxDescAlloc(void);
extern void gfxSetScissor(u16, u16, u16, u16);
extern void gfxSetViewport(u16, u16, u16, u16);
extern void _gfxSetFlagsImm(s32);
extern void gfxSetUpdates(u32, u32, u32);
extern void gfxSetZMode(u32, u32, u32);
extern void gfxSetAlphaCompare(u32, u32, u32, u32, u32, u32);
extern void gfxSetCullMode(u32);
extern void gfxSetCoPlanar(u32);
extern void gfxSetDither(u32);
extern void fn_800D9ED8(u32);
extern void gfxSetTevStage(u32, u32, u32, u32, u32);
extern void gfxSetPerspective(f32, f32, f32, f32);
extern void GSgfxBackFBInit__Fv(void);

/*
 * Render modes per TV standard, indexed as fn_800D37D4 computes:
 * 0/1 240-line double-strike (plain/AA), 2/3 240-line interlaced field
 * (plain/AA), 4/5/6 480-line interlaced (plain/deflicker/AA),
 * 7/8 480-line progressive (plain/AA).
 */
static GXRenderModeObj* ntscModes[9] = {
    &lbl_80312C40, &lbl_80312C7C, &lbl_80312CB8, &lbl_80312CF4, &lbl_80312D6C,
    &lbl_80312D30, &lbl_80312DA8, &lbl_80312DE4, &lbl_80312E20,
};
static GXRenderModeObj* palModes[9] = {
    &lbl_80313000, &lbl_8031303C, &lbl_80313078, &lbl_803130B4, &lbl_8031312C,
    &lbl_803130F0, &lbl_80313168, NULL, NULL,
};
static GXRenderModeObj* eurgb60Modes[9] = {
    &lbl_803131A4, &lbl_803131E0, &lbl_8031321C, &lbl_80313258, &lbl_803132D0,
    &lbl_80313294, &lbl_8031330C, NULL, NULL,
};
static GXRenderModeObj* mpalModes[9] = {
    &lbl_80312E5C, &lbl_80312E98, &lbl_80312ED4, &lbl_80312F10, &lbl_80312F88,
    &lbl_80312F4C, &lbl_80312FC4, NULL, NULL,
};

/*
 * Select and apply a video mode.
 *   standard:    1 NTSC, 2 PAL (only if lbl_8047AA9C), 3 EURGB60, 4 MPAL
 *   lines:       1 = 240-line modes, anything else = 480-line modes
 *   interlace:   240-line only: 0 = double-strike, else interlaced field
 *   filter:      1 plain, 2 deflicker, 3 anti-aliased
 *   progressive: 1 = progressive scan (NTSC 480-line only)
 *   retraces:    VI retraces to wait with the display blacked out
 *
 * `index` is left unassigned when a 480-line interlaced call passes a filter
 * other than 1-3, and `selected` when the standard is not 1-4; the retail
 * code has no initialising instruction for either and reads whatever the
 * registers hold (the `interlace` argument and the `lines` argument).
 */
s32 fn_800D37D4(s32 standard, s32 lines, u8 interlace, s32 filter,
                u8 progressive, u16 retraces)
{
    GXRenderModeObj renderMode;
    GXRenderModeObj* selected;
    u32 index;
    s32 i;

    if (progressive == 1) {
        if (standard != 1) {
            return 0;
        }
        if (lines != 2) {
            return 0;
        }
    }
    if (standard == 2) {
        if (lbl_8047AA9C != 1) {
            return 0;
        }
    }

    if (lines == 1) {
        if (interlace == 0) {
            if (filter != 3) {
                index = 0;
            } else {
                index = 1;
            }
        } else {
            if (filter != 3) {
                index = 2;
            } else {
                index = 3;
            }
        }
    } else if (progressive == 0) {
        switch (filter) {
        case 1:
            index = 4;
            break;
        case 2:
            index = 5;
            break;
        case 3:
            index = 6;
            break;
        }
    } else {
        if (filter != 3) {
            index = 7;
        } else {
            index = 8;
        }
    }

    switch (standard) {
    case 1:
        selected = ntscModes[index];
        break;
    case 2:
        selected = palModes[index];
        break;
    case 3:
        selected = eurgb60Modes[index];
        break;
    case 4:
        selected = mpalModes[index];
        break;
    }
    if (selected == NULL) {
        return 0;
    }

    memcpy(&renderMode, selected, sizeof(renderMode));
    GSgfxVideoVsyncRate = standard == 2 ? 50 : 60;
    lbl_8047AA98 = standard;
    if (lbl_8047AAA0 != 0) {
        renderMode.efbHeight = lbl_8047AAA0;
    }
    renderMode.viXOrigin = 30;
    renderMode.viWidth = 660;

    HSD_VISetBlack(1);
    HSD_VISetConfigure(&renderMode);
    for (i = 0; i < retraces; i++) {
        VIWaitForRetrace();
    }
    HSD_VISetBlack(0);
    return 1;
}

/* Initialise the GS graphics core and its default render state. */
void GSgfxInit__FP15_GSgfxInitParms(u32 heapSize, u32 matrixSize,
                                    u32 projectionCount, u32 lightCount,
                                    s32 videoMode, u32 efbHeight)
{
    u16 handle;
    /* All known callers pass modes 1-4; the target has no default assignment. */
    GXRenderModeObj* renderMode;
    u32 previousMode;
    s32 i;
    GXRenderModeObj* display;
    u32* fifoState;
    u8* state;

    GSmathInit();
    state = GSscratchAlloc(3, _gfxScratchNotify__F15GSscratchNotifyPvUc);
    if (state == 0) {
        handle = _toolentryAlloc__FUl(0x5A0);
        if (handle == 0) {
            GSlogWrite(lbl_80270360);
            return;
        }
        state = GSmemLock(handle);
    }

    lbl_8047AA80 = (GSgfxState*)state;
    *(u32*)(state + 0x000) = 2;
    *(s32*)((u8*)lbl_8047AA80 + 0x004) = -1;
    *(u32*)((u8*)lbl_8047AA80 + 0x008) = 0x10;
    *(u32*)((u8*)lbl_8047AA80 + 0x00C) = 0xFEFEFEFE;
    *(u32*)((u8*)lbl_8047AA80 + 0x010) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x014) = 3;
    *(u8*)((u8*)lbl_8047AA80 + 0x018) = 0;
    *(u8*)((u8*)lbl_8047AA80 + 0x019) = 0;
    *(u8*)((u8*)lbl_8047AA80 + 0x01A) = 0;
    *(u8*)((u8*)lbl_8047AA80 + 0x01B) = 0;
    *(u8*)((u8*)lbl_8047AA80 + 0x49C) = 0;
    *(u8*)((u8*)lbl_8047AA80 + 0x49D) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x020) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x024) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x028) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x02C) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x030) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x034) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x038) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x03C) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x040) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x044) = 0;
    *(u8*)((u8*)lbl_8047AA80 + 0x47E) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x480) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x484) = 0;
    *(s32*)((u8*)lbl_8047AA80 + 0x488) = -1;
    *(u8*)((u8*)lbl_8047AA80 + 0x49F) = 0;

    fifoState = (u32*)lbl_804001F0;
    fifoState[0] = 0;
    fifoState[1] = 0;
    fifoState[2] = 0;
    fifoState[3] = 0;
    fifoState[4] = 0;
    fifoState[5] = 0;
    fifoState[6] = 0;
    fifoState[7] = 0;
    fifoState[8] = 0;
    fifoState[9] = 0;
    fifoState[10] = 0;
    fifoState[11] = 0;
    fifoState[12] = 0;
    fifoState[13] = 0;
    fifoState[14] = 0;
    fifoState[15] = 0;
    fifoState[16] = 0;
    fifoState[17] = 0;
    fifoState[18] = 0;
    fifoState[19] = 0;
    fifoState[20] = 0;
    fifoState[21] = 0;

    lbl_8047AAA0 = efbHeight;
    switch (videoMode) {
    case 1:
        renderMode = &lbl_80312D30;
        break;
    case 2:
    case 3:
        renderMode = &lbl_803130F0;
        break;
    case 4:
        renderMode = &lbl_80312F4C;
        break;
    default:
        break;
    }
    if (videoMode == 2 || videoMode == 3) {
        lbl_8047AA9C = 1;
    } else {
        lbl_8047AA9C = 0;
    }

    HSD_SetInitParameter(1, 2);
    HSD_SetInitParameter(4, renderMode);
    HSD_InitComponent();
    fn_800D37D4(videoMode, 2, 0, 2, 0, 0);
    HSD_VISetBlack(1);
    fn_8019C690(0, 0);

    *(u32*)((u8*)lbl_8047AA80 + 0x048) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x04C) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x050) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x054) = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x058) = 1;
    *(u8*)((u8*)lbl_8047AA80 + 0x05C) = 1;
    HSD_VISetUserPostRetraceCallback(fn_800D3F5C);
    HSD_VISetUserGXDrawDoneCallback(fn_800D3F50);
    fn_80196C3C(GSgfxSetViewportJitter);

    gfxAllocRequestBuffer(heapSize);
    gfxMatrixStackInit(matrixSize);
    gfxVtxDescPoolInit(projectionCount);
    fn_800DB890(lightCount);
    *(u32*)((u8*)lbl_8047AA80 + 0x20) = gfxVtxDescAlloc();

    display = &lbl_80466BC0;
    previousMode = *(u32*)((u8*)lbl_8047AA80 + 0x00);
    *(u32*)((u8*)lbl_8047AA80 + 0x00) = 2;
    gfxSetScissor(0, 0, display->fbWidth - 1, display->efbHeight - 1);
    gfxSetViewport(0, 0, display->fbWidth - 1, display->efbHeight - 1);
    _gfxSetFlagsImm(-1);
    gfxSetUpdates(1, 1, 1);
    gfxSetZMode(1, 2, 1);
    gfxSetAlphaCompare(0, 7, 0, 1, 7, 0);
    gfxSetCullMode(2);
    gfxSetCoPlanar(0);
    gfxSetDither(1);
    fn_800D9ED8(0);

    for (i = 0; i < 0x10; i++) {
        gfxSetTevStage(i, 0, i, i, 0);
    }
    gfxSetPerspective(lbl_8047CA10, lbl_8047CA14, lbl_8047CA18, lbl_8047CA1C);
    state = (u8*)lbl_8047AA80;
    *(u32*)(state + 0x00) = previousMode;
    lbl_8047AA8C = 0;
    *(u32*)((u8*)lbl_8047AA80 + 0x00) = 1;
    GSgfxBackFBInit__Fv();
    HSD_VISetBlack(0);
    lbl_8047AA84 = 1;
    lbl_8047AA85 = 0;
    GSlogWrite(lbl_80270388, lbl_8047AA80, 0x5A0);
}
