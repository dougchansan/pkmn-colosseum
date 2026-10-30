/**
 * @file video_exact_801BFA1C.c
 * @brief HAL video.c, tail of the retail text: HSD_VICopyEFB2XFBPtr through
 *        the user-callback setters, 0x801BFA1C-0x801C0270.
 *
 * Text-only carve of src/hsd/video.c; see video_exact_801BF1F0.c for why
 * the TU is split around 0x801BF6AC-0x801BFA1C. video.c's data stays in its data
 * units and is referenced by its dtk names: lbl_8047DF30 ("video.c"),
 * lbl_8047DF38 (1.0F), lbl_802756F8 ("idx != -1"), lbl_8027575C (the
 * render-pass panic message), lbl_804657C0 (the anti-aliasing garbage XFB)
 * and lbl_8047B380/lbl_8047B384 (HSD_VIPreRetraceCB's static vr_count and
 * renew_count). Functions are listed in HAL's order; MWCC emits them in
 * reverse.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OSInterrupt.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/video.h"

/* GX enum values used here (this tree's GX.h does not define them) */
#define GX_TRUE 1
#define GX_LEQUAL 3
#define GX_CLAMP_TOP 1
#define GX_CLAMP_BOTTOM 2

extern const char lbl_8047DF30[8]; /* "video.c" */
extern const f32 lbl_8047DF38;     /* 1.0F */
extern const char lbl_802756F8[];  /* "idx != -1" */
extern const char lbl_8027575C[];  /* "unexpected type of render pass.\n" */
extern u8 lbl_804657C0[HSD_ANTIALIAS_GARBAGE_SIZE]; /* garbage */
extern int lbl_8047B380;           /* HSD_VIPreRetraceCB: vr_count */
extern int lbl_8047B384;           /* HSD_VIPreRetraceCB: renew_count */

#define VIDEO_ASSERT(line, cond, str) \
    ((cond) ? ((void) 0) : __assert(lbl_8047DF30, line, str))

void VIConfigure(GXRenderModeObj* rm);
void VIFlush(void);
void VISetNextFrameBuffer(void* fb);
void VISetBlack(BOOL black);

void GXPixModeSync(void);                         /* GXPixModeSync */
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
            VIDEO_ASSERT(252, idx != -1, lbl_802756F8);
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

    if (renew) {
        lbl_8047B384++;
    }
    if (++lbl_8047B380 >= _p->perf.frame_period) {
        _p->perf.frame_renew = lbl_8047B384;
        lbl_8047B380 = lbl_8047B384 = 0;
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
int HSD_VIGetDrawDoneWaitingFlag(void)
{
    return _p->drawdone.waiting;
}

static void HSD_VICopyEFB2XFBHiResoAA(GXRenderModeObj* rmode)
{
    int n_xfb_lines;

    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight - HSD_ANTIALIAS_OVERLAP);
    n_xfb_lines = GXSetDispCopyYScale(lbl_8047DF38);
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
        HSD_Panic(lbl_8047DF30, 537, lbl_8027575C);
    }

    GXPixModeSync();
}
