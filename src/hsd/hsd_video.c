/**
 * @file hsd_video.c
 * @brief sysdolphin video.c tail, 0x801BF6AC - 0x801C01C8.
 *
 * The bodies follow the canonical sysdolphin video implementation.  The
 * retail addresses and embedded "video.c" panic identify this TU.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/vi/VI.h"

#define HSD_VI_XFB_MAX 3
#define HSD_ANTIALIAS_OVERLAP 4

typedef void (*HSD_VIGXDrawDoneCallback)(int);

typedef enum HSD_VIXFBDrawDispStatus {
    HSD_VI_XFB_NONE,
    HSD_VI_XFB_NOUSE,
    HSD_VI_XFB_FREE,
    HSD_VI_XFB_DRAWING,
    HSD_VI_XFB_WAITDONE,
    HSD_VI_XFB_DRAWDONE,
    HSD_VI_XFB_NEXT,
    HSD_VI_XFB_DISPLAY,
    HSD_VI_XFB_COPYEFB,
    HSD_VI_XFB_TERMINATE
} HSD_VIXFBDrawDispStatus;

typedef enum HSD_VIEFBDrawDispStatus {
    HSD_VI_EFB_FREE,
    HSD_VI_EFB_DRAWDONE,
    HSD_VI_EFB_TERMINATE
} HSD_VIEFBDrawDispStatus;

typedef enum HSD_RenderPass {
    HSD_RP_SCREEN,
    HSD_RP_TOPHALF,
    HSD_RP_BOTTOMHALF,
    HSD_RP_OFFSCREEN
} HSD_RenderPass;

typedef struct HSD_VIStatus {
    GXRenderModeObj rmode;
    s32 black;
    u8 vf;
    s32 gamma;
    GXColor clear_clr;
    u32 clear_z;
    u8 update_clr;
    u8 update_alpha;
    u8 update_z;
} HSD_VIStatus;

typedef struct Current {
    HSD_VIStatus vi;
    u8 chg_flag;
} Current;

typedef struct XFB {
    void* buffer;
    HSD_VIXFBDrawDispStatus status;
    Current vi_all;
} XFB;

typedef struct HSD_VIInfo {
    Current current;
    XFB xfb[HSD_VI_XFB_MAX];
    struct {
        HSD_VIEFBDrawDispStatus status;
        Current vi_all;
    } efb;
    s32 nb_xfb;
    VIRetraceCallback pre_cb;
    VIRetraceCallback post_cb;
    struct {
        s32 waiting;
        s32 arg;
        HSD_VIGXDrawDoneCallback cb;
    } drawdone;
    struct {
        s32 frame_period;
        s32 frame_renew;
    } perf;
} HSD_VIInfo;

extern HSD_VIInfo lbl_80466BC0;
extern u8 lbl_804657C0[];
extern char lbl_8047DF30[];
extern char lbl_802756F8[];
extern char lbl_80275704[];
extern char lbl_8027575C[];

#define _p (&lbl_80466BC0)

extern void fn_800B959C(u16 left, u16 top, u16 width, u16 height);
extern void fn_800B96BC(u16 width, u16 height);
extern void fn_800B9874(u32 clamp);
extern u32 fn_800B9B14(f32 scale);
extern void fn_800B9BDC(GXColor color, u32 clear_z);
extern void fn_800B9C44(BOOL aa, u8 sample_pattern[12][2], BOOL vf,
                        u8 vfilter[7]);
extern void fn_800B9E6C(u32 gamma);
extern void fn_800B9E88(void* buffer, BOOL clear);
extern void fn_800B8E74(void);
extern void fn_801B278C(BOOL enable);
extern void fn_801B273C(BOOL enable);
extern void fn_801B27DC(BOOL enable, u32 func, BOOL update);
extern void GXSetDrawDone(void);
extern void GXWaitDrawDone(void);
extern void __assert(const char* file, u32 line, const char* expr);
extern void HSD_Panic(const char* file, u32 line, const char* msg);

static inline int HSD_VISearchXFBByStatus(HSD_VIXFBDrawDispStatus status)
{
    int i;
    for (i = 0; i < HSD_VI_XFB_MAX; i++) {
        if (_p->xfb[i].status == status) {
            return i;
        }
    }
    return -1;
}

static inline int HSD_VIGetXFBDrawEnable(void)
{
    BOOL intr;
    int idx = -1;

    if (_p->nb_xfb < 2) {
        return idx;
    }
    intr = OSDisableInterrupts();
    if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING)) == -1) {
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE)) != -1) {
            _p->xfb[idx].status = HSD_VI_XFB_DRAWING;
        }
    }
    OSRestoreInterrupts(intr);
    return idx;
}

static inline int HSD_VIWaitXFBDrawEnable(void)
{
    int idx = -1;
    if (_p->nb_xfb < 2) {
        return idx;
    }
    while ((idx = HSD_VIGetXFBDrawEnable()) == -1) {
        VIWaitForRetrace();
    }
    return idx;
}

static inline int HSD_VIWaitXFBFlushSub(void)
{
    BOOL intr;
    int val;
    intr = OSDisableInterrupts();
    val = HSD_VISearchXFBByStatus(HSD_VI_XFB_WAITDONE) != -1 ||
          HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWDONE) != -1 ||
          HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT) != -1;
    OSRestoreInterrupts(intr);
    return val;
}

/* HSD_VIWaitXFBFlush */
void fn_801BF6AC(void)
{
    if (_p->nb_xfb < 2) {
        return;
    }
    while (HSD_VIWaitXFBFlushSub()) {
        VIWaitForRetrace();
    }
}

void fn_801BFA1C(HSD_VIStatus* vi, void* buffer, HSD_RenderPass rpass);

/* HSD_VICopyXFBAsync */
void fn_801BF8A0(HSD_RenderPass rpass)
{
    BOOL intr;
    int idx;

    if (_p->nb_xfb < 2) {
        return;
    }
    idx = HSD_VIWaitXFBDrawEnable();
    fn_801BFA1C(&_p->current.vi, _p->xfb[idx].buffer, rpass);

    intr = OSDisableInterrupts();
    if (_p->xfb[idx].status != HSD_VI_XFB_DRAWING) {
        __assert(lbl_8047DF30, 590, lbl_80275704);
    }
    _p->xfb[idx].status = HSD_VI_XFB_WAITDONE;
    _p->xfb[idx].vi_all = _p->current;
    _p->current.chg_flag = 0;
    OSRestoreInterrupts(intr);

    while (fn_801BFCA0()) {
        GXWaitDrawDone();
    }
    _p->drawdone.waiting = 1;
    _p->drawdone.arg = idx;
    GXSetDrawDone();
}

/* HSD_VICopyEFB2XFBPtr */
void fn_801BFA1C(HSD_VIStatus* vi, void* buffer, HSD_RenderPass rpass)
{
    GXRenderModeObj* rmode = &vi->rmode;
    int n_xfb_lines;
    u16 lines;
    u32 offset;

    fn_800B9C44(rmode->aa, rmode->sample_pattern, vi->vf, rmode->vfilter);
    fn_800B9E6C(vi->gamma);
    fn_801B278C(vi->update_clr);
    fn_801B273C(vi->update_alpha);
    fn_801B27DC(vi->update_z, 3, TRUE);
    fn_800B9BDC(vi->clear_clr, vi->clear_z);

    switch (rpass) {
    case HSD_RP_SCREEN:
        fn_800B9874(3);
        fn_800B959C(0, 0, rmode->fbWidth, rmode->efbHeight);
        n_xfb_lines = fn_800B9B14((f32) rmode->xfbHeight /
                                  (f32) rmode->efbHeight);
        fn_800B96BC(rmode->fbWidth, n_xfb_lines);
        fn_800B9E88(buffer, TRUE);
        break;
    case HSD_RP_TOPHALF:
        fn_800B959C(0, 0, rmode->fbWidth,
                    rmode->efbHeight - HSD_ANTIALIAS_OVERLAP);
        n_xfb_lines = fn_800B9B14(1.0F);
        fn_800B96BC(rmode->fbWidth, n_xfb_lines);
        fn_800B9874(1);
        lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
        fn_800B959C(0, 0, rmode->fbWidth, lines);
        fn_800B9E88(buffer, TRUE);
        fn_800B8E74();
        return;
    case HSD_RP_BOTTOMHALF:
        fn_800B959C(0, 0, rmode->fbWidth,
                    rmode->efbHeight - HSD_ANTIALIAS_OVERLAP);
        n_xfb_lines = fn_800B9B14(1.0F);
        fn_800B96BC(rmode->fbWidth, n_xfb_lines);
        fn_800B9874(2);
        lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
        fn_800B959C(0, HSD_ANTIALIAS_OVERLAP, rmode->fbWidth, lines);
        offset = (((rmode->fbWidth + 15) & ~15) * lines * 2);
        fn_800B9E88((u8*) buffer + offset, TRUE);
        fn_800B959C(0, 0, rmode->fbWidth, HSD_ANTIALIAS_OVERLAP);
        fn_800B9874(3);
        fn_800B9E88(lbl_804657C0, TRUE);
        break;
    default:
        HSD_Panic(lbl_8047DF30, 0x219, lbl_8027575C);
    }
    fn_800B8E74();
}

int fn_801BFCA0(void)
{
    return _p->drawdone.waiting;
}

void fn_801BFCB0(void)
{
    _p->drawdone.waiting = 0;
    if (_p->drawdone.cb) {
        _p->drawdone.cb(_p->drawdone.arg);
    }
}

void fn_801BFD10(u32 retraceCount)
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
        fn_801BFA1C(&_p->efb.vi_all.vi, _p->xfb[idx].buffer, HSD_RP_SCREEN);
        _p->xfb[idx].status = HSD_VI_XFB_DISPLAY;
        _p->efb.status = HSD_VI_EFB_FREE;
    }
    if (_p->post_cb) {
        _p->post_cb(retraceCount);
    }
}

void fn_801BFF18(u32 retraceCount)
{
    int idx;
    int flush = 0;
    int renew = 0;
    static int vr_count;
    static int renew_count;

    idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT);
    if (idx != -1) {
        VISetNextFrameBuffer(_p->xfb[idx].buffer);
        if (_p->xfb[idx].vi_all.chg_flag) {
            VIConfigure(&_p->xfb[idx].vi_all.vi.rmode);
            VISetBlack(_p->xfb[idx].vi_all.vi.black);
        }
        flush = 1;
        renew = 1;
    } else if (_p->nb_xfb == 1 && _p->efb.status == HSD_VI_EFB_DRAWDONE) {
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DISPLAY)) == -1) {
            idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE);
            if (idx == -1) {
                __assert(lbl_8047DF30, 252, lbl_802756F8);
            }
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
        renew_count++;
    }
    if (++vr_count >= _p->perf.frame_period) {
        _p->perf.frame_renew = renew_count;
        vr_count = renew_count = 0;
    }
    if (_p->pre_cb) {
        _p->pre_cb(retraceCount);
    }
}
