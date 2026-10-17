/**
 * @file video.c
 * @brief HAL sysdolphin video.c: VI retrace callbacks, XFB bookkeeping and
 *        the EFB-to-XFB copy, 0x801BF1F0-0x801C0270.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/video.c, commit 1e4b3b5adc74e52e420a86b3dd0da4bb67867cee)
 * and checked against Colosseum's retail
 * code, which is the newer sysdolphin: a full-screen copy that yields fewer
 * XFB lines than the XFB holds fills the rest with black, and the library
 * gained a "finish the XFB being drawn" entry point (HSD_VIFinishDrawXFB) that
 * marks it waiting and then drawn under one interrupt lock.
 *
 * The library is built with deferred inlining, so functions are listed in
 * HAL's order and MWCC emits them in reverse (the retail address order).
 * The complete video object is linked with its own literals and state. The
 * locally tagged dont_inline pragma below follows Melee's same accessor
 * boundary and is a title-path exception, not a strict campaign win.
 * Functions nothing in the game references are compiled and dead-stripped
 * by the linker as in retail. Globals other objects link against keep
 * their dtk names; the comments give the HAL names. SDK calls that are
 * fn_ symbols in Colosseum's map carry their SDK names in comments.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OSInterrupt.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/video.h"

/* GX enum values used here (this tree's GX.h does not define them) */
#define GX_FALSE 0
#define GX_TRUE 1
#define GX_LEQUAL 3
#define GX_CLAMP_TOP 1
#define GX_CLAMP_BOTTOM 2

void VIInit(void);
void VIWaitForRetrace(void);
void VIConfigure(GXRenderModeObj* rm);
void VIFlush(void);
void VISetNextFrameBuffer(void* fb);
void VISetBlack(BOOL black);
u32 VIGetTvFormat(void);
HSD_VIRetraceCallback VISetPreRetraceCallback(HSD_VIRetraceCallback cb); /* VISetPreRetraceCallback */
HSD_VIRetraceCallback VISetPostRetraceCallback(HSD_VIRetraceCallback cb); /* VISetPostRetraceCallback */

void GXWaitDrawDone(void);                         /* GXWaitDrawDone */
void GXDrawDone(void);                          /* GXSetDrawDone */
void GXPixModeSync(void);                         /* GXPixModeSync */
void GXSetDrawDoneCallback(void (*cb)(void));             /* GXSetDrawDoneCallback */
void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht); /* GXSetDispCopySrc */
void GXSetDispCopyDst(u16 wd, u16 ht);               /* GXSetDispCopyDst */
void GXSetCopyClamp(u32 clamp);                    /* GXSetCopyClamp */
f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight);
u32 GXSetDispCopyYScale(f32 vscale);                    /* GXSetDispCopyYScale */
void GXSetCopyClear(GXColor clear_clr, u32 clear_z); /* GXSetCopyClear */
void GXSetCopyFilter(GXBool aa, u8 sample_pattern[12][2], GXBool vf,
                 u8 vfilter[7]);                /* GXSetCopyFilter */
void GXSetDispCopyGamma(s32 gamma);                    /* GXSetDispCopyGamma */
void GXCopyDisp(void* dest, GXBool clear);     /* GXCopyDisp */

void HSD_StateSetAlphaUpdate(u8 enable);                    /* HSD_StateSetAlphaUpdate */
void HSD_StateSetColorUpdate(u8 enable);                    /* HSD_StateSetColorUpdate */
void HSD_StateSetZMode(u8 enable, s32 func, u8 update); /* HSD_StateSetZMode */

/* HAL: HSD_VIData */
HSD_VIInfo lbl_80466BC0;
/* HAL: garbage, the copy target for the anti-aliasing overlap lines */
static u8 lbl_804657C0[HSD_ANTIALIAS_GARBAGE_SIZE] __attribute__((aligned(32)));

#define _p ((HSD_VIInfo*) &lbl_80466BC0)

static int HSD_VISearchXFBByStatus(HSD_VIXFBDrawDispStatus status)
{
    int i;

    for (i = 0; i < HSD_VI_XFB_MAX; i++) {
        if (_p->xfb[i].status == status) {
            return i;
        }
    }
    return -1;
}

HSD_VIRetraceCallback HSD_VISetUserPreRetraceCallback(HSD_VIRetraceCallback cb)
{
    BOOL intr;
    HSD_VIRetraceCallback old = _p->pre_cb;

    intr = OSDisableInterrupts();
    _p->pre_cb = cb;
    OSRestoreInterrupts(intr);

    return old;
}

/* HAL: HSD_VISetUserPostRetraceCallback */
HSD_VIRetraceCallback HSD_VISetUserPostRetraceCallback(HSD_VIRetraceCallback cb)
{
    BOOL intr;
    HSD_VIRetraceCallback old = _p->post_cb;

    intr = OSDisableInterrupts();
    _p->post_cb = cb;
    OSRestoreInterrupts(intr);

    return old;
}

/* HAL: HSD_VISetUserGXDrawDoneCallback */
HSD_VIGXDrawDoneCallback HSD_VISetUserGXDrawDoneCallback(HSD_VIGXDrawDoneCallback cb)
{
    BOOL intr;
    HSD_VIGXDrawDoneCallback old = _p->drawdone.cb;

    intr = OSDisableInterrupts();
    _p->drawdone.cb = cb;
    OSRestoreInterrupts(intr);

    return old;
}

/* HAL: HSD_VIPreRetraceCB */
void HSD_VIPreRetraceCB(u32 retraceCount)
{
    int idx;
    int flush = 0;
    int renew = 0;

    idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT);
    if (idx != -1) {
        VISetNextFrameBuffer(_p->xfb[idx].buffer);
        if (_p->xfb[idx].vi_all.chg_flag) {
            VIConfigure(&_p->xfb[idx].vi_all.vi.rmode);
            VISetBlack(_p->xfb[idx].vi_all.vi.black);
        }
        flush = 1;
        renew = 1;
    } else if (HSD_VIGetNbXFB() == 1 && _p->efb.status == HSD_VI_EFB_DRAWDONE)
    {
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DISPLAY)) == -1) {
            idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE);
            HSD_ASSERT(252, idx != -1);
            VISetNextFrameBuffer(_p->xfb[idx].buffer);
            flush = 1;
        }
        _p->xfb[idx].status = HSD_VI_XFB_COPYEFB;
        if (_p->efb.vi_all.chg_flag) {
            VIConfigure(&_p->efb.vi_all.vi.rmode);
            VISetBlack(_p->efb.vi_all.vi.black);
            flush = 1;
        }
        renew = 1;
    }

    if (flush) {
        VIFlush();
    }

    {
        static int vr_count = 0;
        static int renew_count = 0;

        if (renew) {
            renew_count++;
        }
        if (++vr_count >= _p->perf.frame_period) {
            _p->perf.frame_renew = renew_count;
            vr_count = renew_count = 0;
        }
    }

    if (_p->pre_cb) {
        _p->pre_cb(retraceCount);
    }
}

/* HAL: HSD_VIPostRetraceCB */
void HSD_VIPostRetraceCB(u32 retraceCount)
{
    int idx;
    int next;

    if ((next = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT)) != -1) {
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DISPLAY)) != -1) {
            _p->xfb[idx].status = HSD_VI_XFB_FREE;
        }
        _p->xfb[next].status = HSD_VI_XFB_DISPLAY;
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWDONE)) != -1) {
            _p->xfb[idx].status = HSD_VI_XFB_NEXT;
        }
    } else if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_COPYEFB)) != -1) {
        HSD_VICopyEFB2XFB(&_p->efb.vi_all.vi, _p->xfb[idx].buffer, HSD_RP_SCREEN);
        _p->xfb[idx].status = HSD_VI_XFB_DISPLAY;
        _p->efb.status = HSD_VI_EFB_FREE;
    }

    if (_p->post_cb) {
        _p->post_cb(retraceCount);
    }
}

/* HAL: HSD_VIGXDrawDoneCB */
void HSD_VIGXDrawDoneCB(void)
{
    _p->drawdone.waiting = 0;

    if (_p->drawdone.cb) {
        _p->drawdone.cb(_p->drawdone.arg);
    }
}

/* HAL: HSD_VIGetDrawDoneWaitingFlag */
#pragma push
#pragma dont_inline on /* RULE-EXCEPTION(title-path): local compiler control - see docs/RULE_EXCEPTIONS.md */
int HSD_VIGetDrawDoneWaitingFlag(void)
{
    return _p->drawdone.waiting;
}
#pragma pop

int HSD_VIGetXFBDrawEnable(void)
{
    BOOL intr;
    int idx = -1;

    if (HSD_VIGetNbXFB() < 2) {
        goto ret;
    }

    intr = OSDisableInterrupts();

    if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING)) == -1) {
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE)) != -1) {
            _p->xfb[idx].status = HSD_VI_XFB_DRAWING;
        }
    }

    OSRestoreInterrupts(intr);

ret:
    return idx;
}

int HSD_VIWaitXFBDrawEnable(void)
{
    int idx = -1;

    if (HSD_VIGetNbXFB() < 2) {
        goto ret;
    }

    while ((idx = HSD_VIGetXFBDrawEnable()) == -1) {
        VIWaitForRetrace();
    }

ret:
    return idx;
}

static void HSD_VICopyEFB2XFBHiResoAA(GXRenderModeObj* rmode)
{
    int n_xfb_lines;

    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight - HSD_ANTIALIAS_OVERLAP);
    n_xfb_lines = GXSetDispCopyYScale(1.0F);
    GXSetDispCopyDst(rmode->fbWidth, n_xfb_lines);
}

/* HAL: HSD_VICopyEFB2XFBPtr */
void HSD_VICopyEFB2XFB(HSD_VIStatus* vi, void* buffer, HSD_RenderPass rpass)
{
    GXRenderModeObj* rmode = &vi->rmode;
    int n_xfb_lines;
    u16 lines;
    u32 offset;

    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, vi->vf, rmode->vfilter);
    GXSetDispCopyGamma(vi->gamma);

    HSD_StateSetColorUpdate(vi->update_clr);
    HSD_StateSetAlphaUpdate(vi->update_alpha);
    HSD_StateSetZMode(vi->update_z, GX_LEQUAL, GX_TRUE);

    GXSetCopyClear(vi->clear_clr, vi->clear_z);

    switch (rpass) {
    case HSD_RP_SCREEN:
        GXSetCopyClamp(GX_CLAMP_TOP | GX_CLAMP_BOTTOM);
        GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
        n_xfb_lines = GXSetDispCopyYScale(
            GXGetYScaleFactor(rmode->efbHeight, rmode->xfbHeight));
        GXSetDispCopyDst(rmode->fbWidth, n_xfb_lines);
        GXCopyDisp(buffer, GX_TRUE);
        {
            u32 rest = rmode->xfbHeight - n_xfb_lines;

            if (rest != 0) {
                u32 stride =
                    VIPadFrameBufferWidth(rmode->fbWidth) * VI_DISPLAY_PIX_SZ;
                u32 n;
                u32* p;

                p = (u32*) ((u32) buffer + stride * n_xfb_lines);
                n = (rest * stride) / 4;

                while (n--) {
                    *p++ = 0x10801080;
                }
            }
        }
        break;

    case HSD_RP_TOPHALF:
        HSD_VICopyEFB2XFBHiResoAA(rmode);
        GXSetCopyClamp(GX_CLAMP_TOP);
        lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
        GXSetDispCopySrc(0, 0, rmode->fbWidth, lines);
        GXCopyDisp(buffer, GX_TRUE);
        GXPixModeSync();
        return;

    case HSD_RP_BOTTOMHALF:
        HSD_VICopyEFB2XFBHiResoAA(rmode);
        GXSetCopyClamp(GX_CLAMP_BOTTOM);
        lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
        GXSetDispCopySrc(0, HSD_ANTIALIAS_OVERLAP, rmode->fbWidth, lines);
        offset = (VIPadFrameBufferWidth(rmode->fbWidth) * lines *
                  (u32) VI_DISPLAY_PIX_SZ);
        GXCopyDisp((void*) ((u32) buffer + offset), GX_TRUE);
        GXSetDispCopySrc(0, 0, rmode->fbWidth, HSD_ANTIALIAS_OVERLAP);
        GXSetCopyClamp(GX_CLAMP_TOP | GX_CLAMP_BOTTOM);
        GXCopyDisp((void*) lbl_804657C0, GX_TRUE);
        break;

    default:
        HSD_Panic(__FILE__, 537, "unexpected type of render pass.\n");
    }

    GXPixModeSync();
}

void HSD_VIGXSetDrawDone(int arg)
{
    while (HSD_VIGetDrawDoneWaitingFlag()) {
        GXWaitDrawDone();
    }
    _p->drawdone.waiting = 1;
    _p->drawdone.arg = arg;
    GXDrawDone();
}

void HSD_VISetXFBWaitDone(int idx)
{
    BOOL intr;

    intr = OSDisableInterrupts();

    HSD_ASSERT(608, _p->xfb[idx].status == HSD_VI_XFB_DRAWING);

    _p->xfb[idx].status = HSD_VI_XFB_WAITDONE;
    _p->xfb[idx].vi_all = _p->current;
    _p->current.chg_flag = 0;

    OSRestoreInterrupts(intr);
}

/* HAL: HSD_VICopyXFBAsync */
void fn_801BF8A0(HSD_RenderPass rpass)
{
    int idx;

    if (HSD_VIGetNbXFB() < 2) {
        return;
    }

    idx = HSD_VIWaitXFBDrawEnable();
    HSD_VICopyEFB2XFB(HSD_VIGetVIStatus(), HSD_VIGetXFBPtr(idx), rpass);

    HSD_VIGXSetDrawDone(idx);
}

void HSD_VIDrawDoneXFB(int idx)
{
    BOOL intr;

    intr = OSDisableInterrupts();

    HSD_ASSERT(740, _p->xfb[idx].status == HSD_VI_XFB_WAITDONE);

    _p->xfb[idx].status = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT) != -1
                              ? HSD_VI_XFB_DRAWDONE
                              : HSD_VI_XFB_NEXT;

    OSRestoreInterrupts(intr);
}

static int HSD_VIWaitXFBFlush_sub(void)
{
    BOOL intr;
    int val;

    intr = OSDisableInterrupts();

    val = (HSD_VISearchXFBByStatus(HSD_VI_XFB_WAITDONE) != -1 ||
           HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWDONE) != -1 ||
           HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT) != -1)
              ? 1
              : 0;

    OSRestoreInterrupts(intr);

    return val;
}

void HSD_VIWaitXFBFlush(void)
{
    if (HSD_VIGetNbXFB() < 2) {
        return;
    }

    while (HSD_VIWaitXFBFlush_sub()) {
        VIWaitForRetrace();
    }
}

void HSD_VIWaitXFBFlushNoYield(void)
{
    if (HSD_VIGetNbXFB() < 2) {
        return;
    }

    while (HSD_VIWaitXFBFlush_sub()) {
    }
}

/* Finishes the XFB being drawn: marks it waiting, then drawn (newer
 * sysdolphin; no Melee counterpart). */
void HSD_VIFinishDrawXFB(void)
{
    BOOL intr;
    int idx;

    if (HSD_VIGetNbXFB() < 2) {
        return;
    }

    intr = OSDisableInterrupts();
    idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING);
    HSD_ASSERT(781, idx != -1);
    HSD_VISetXFBWaitDone(idx);
    HSD_VIDrawDoneXFB(idx);
    OSRestoreInterrupts(intr);
}

/* HAL: HSD_VIGetXFBLastDrawDone */
int HSD_VIGetXFBLastDrawDone(void)
{
    BOOL intr;
    int idx = -1;

    intr = OSDisableInterrupts();

    if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_WAITDONE)) == -1) {
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWDONE)) == -1) {
            if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT)) == -1) {
                idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DISPLAY);
            }
        }
    }

    OSRestoreInterrupts(intr);

    return idx;
}

/* HAL: HSD_VISetConfigure */
void HSD_VISetConfigure(GXRenderModeObj* rmode)
{
    _p->current.vi.rmode = *rmode;
    _p->current.chg_flag = 1;
}

/* HAL: HSD_VISetBlack */
void HSD_VISetBlack(BOOL black)
{
    _p->current.vi.black = black;
    _p->current.chg_flag = 1;
}

/* HAL: HSD_VIInit */
void HSD_VIInit(HSD_VIStatus* vi, void* xfb0, void* xfb1, void* xfb2)
{
    int i, fbnum, idx;

    VIInit();

    _p->current.vi = *vi;
    _p->current.chg_flag = 0;
    _p->xfb[0].buffer = xfb0;
    _p->xfb[1].buffer = xfb1;
    _p->xfb[2].buffer = xfb2;

    for (i = 0, fbnum = 0; i < HSD_VI_XFB_MAX; i++) {
        _p->xfb[i].vi_all = _p->current;
        if (_p->xfb[i].buffer) {
            fbnum++;
            _p->xfb[i].status = HSD_VI_XFB_FREE;
        } else {
            _p->xfb[i].status = HSD_VI_XFB_NONE;
        }
    }

    _p->nb_xfb = fbnum;

    _p->efb.status = HSD_VI_EFB_FREE;
    _p->efb.vi_all = _p->current;

    VISetPreRetraceCallback(HSD_VIPreRetraceCB);
    VISetPostRetraceCallback(HSD_VIPostRetraceCB);

    _p->pre_cb = NULL;
    _p->post_cb = NULL;

    _p->drawdone.waiting = 0;
    _p->drawdone.arg = 0;

    GXSetDrawDoneCallback(HSD_VIGXDrawDoneCB);
    _p->drawdone.cb = NULL;

    _p->perf.frame_period = VIGetTvFormat() == 0 ? 60 : 50;
    _p->perf.frame_renew = 0;

    VIConfigure(&_p->current.vi.rmode);
    VISetBlack(_p->current.vi.black);
    VIFlush();

    idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE);
    HSD_VICopyEFB2XFB(HSD_VIGetVIStatus(), HSD_VIGetXFBPtr(idx), HSD_RP_SCREEN);
}
