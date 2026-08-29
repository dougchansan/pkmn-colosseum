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

/*
 * The retail TU was built with the global optimizer disabled: with it on,
 * MWCC canonicalises `&lbl_80466BC0 + i * sizeof(XFB)` with the constant
 * address on the right and evaluates the scaled index first, which swaps the
 * base/offset temporaries in every `xfb[]` access.  Turning it off restores
 * the retail base-first allocation across the whole file.
 *
 * This must stay at file scope.  Do not narrow it to push/pop pairs around
 * individual functions the way effect_visual.c and
 * fight_trainer_ai_waza_damage.c scope their pragmas.  Every function here
 * that touches _p->xfb[] needs it, including ones with no other reason to be
 * edited: re-enabling the optimizer for fn_801BFD10 alone, via
 * `#pragma push` / `#pragma global_optimizer on` / `#pragma pop` around its
 * body, regresses it from 100% to 97.115% (measured).
 *
 * There is no command-line equivalent to move this into the build config.
 * mwcceppc's -opt keyword table (off/on/all/space/speed, level=0..4, cse,
 * deadcode, deadstore, dead, lifetimes, loop, prop, strength, peep, schedule,
 * inter/local/unroll) has no global-optimizer entry, and even `-opt level=0`
 * still runs the global optimizer for temporaries, so it is not the same
 * thing -- it scores 90.97% on fn_801BFD10 against the pragma's 100%.
 */
#pragma global_optimizer off

#define HSD_VI_XFB_MAX 3
#define HSD_ANTIALIAS_OVERLAP 4

/* Two YUY2 pixels of XFB black (Y=0x10, Cb=Cr=0x80). */
#define HSD_VI_XFB_BLACK2 0x10801080

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
extern const char lbl_8047DF30[8];
extern char lbl_802756F8[];
extern char lbl_80275704[];
extern char lbl_8027575C[];
extern const f32 lbl_8047DF38[2]; /* { 1.0f, 0.0f } */
extern int lbl_8047B380;
extern int lbl_8047B384;

/* HSD_VIPreRetraceCB's frame-period counters, in .sbss outside this split. */
#define vr_count lbl_8047B380
#define renew_count lbl_8047B384

#define _p (&lbl_80466BC0)

extern void fn_800B959C(u16 left, u16 top, u16 width, u16 height);
extern void fn_800B96BC(u16 width, u16 height);
extern void fn_800B9874(u32 clamp);
extern f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight);
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
extern void fn_800B8DA8(void);
extern void GXDrawDone(void);
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
    int idx = -1;

    if (_p->nb_xfb >= 2) {
        BOOL intr = OSDisableInterrupts();
        if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING)) == -1) {
            if ((idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE)) != -1) {
                _p->xfb[idx].status = HSD_VI_XFB_DRAWING;
            }
        }
        OSRestoreInterrupts(intr);
    }
    return idx;
}

static inline int HSD_VIWaitXFBDrawEnable(void)
{
    int idx = -1;
    if (_p->nb_xfb >= 2) {
        while ((idx = HSD_VIGetXFBDrawEnable()) == -1) {
            VIWaitForRetrace();
        }
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

/* Finalize the active XFB draw and advance its state. */
void fn_801BF6AC(void)
{
    /*
     * The three assert expressions are one -str reuse literal pool, and the
     * retail code keeps its base in a callee-saved register for the whole
     * function.  The pool lives in .rodata that splits.txt gives to
     * game/data/rodata_80270008.c, so it has to be reached through the extern
     * rather than written back as string literals here.
     */
    const char* msg = lbl_802756F8;
    BOOL inner;
    BOOL intr;
    int idx;

    if (_p->nb_xfb < 2) {
        return;
    }
    intr = OSDisableInterrupts();
    idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_DRAWING);
    if (idx == -1) {
        __assert(lbl_8047DF30, 0x30D, msg + 0x0);
    }
    inner = OSDisableInterrupts();
    if (_p->xfb[idx].status != HSD_VI_XFB_DRAWING) {
        __assert(lbl_8047DF30, 0x260, msg + 0xC);
    }
    _p->xfb[idx].status = HSD_VI_XFB_WAITDONE;
    _p->xfb[idx].vi_all = _p->current;
    _p->current.chg_flag = 0;
    OSRestoreInterrupts(inner);

    inner = OSDisableInterrupts();
    if (_p->xfb[idx].status != HSD_VI_XFB_WAITDONE) {
        __assert(lbl_8047DF30, 0x2E4, msg + 0x38);
    }
    _p->xfb[idx].status = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT) == -1
                              ? HSD_VI_XFB_NEXT
                              : HSD_VI_XFB_DRAWDONE;
    OSRestoreInterrupts(inner);
    OSRestoreInterrupts(intr);
}

void fn_801BFA1C(HSD_VIStatus* vi, void* buffer, HSD_RenderPass rpass);

/* HSD_VICopyXFBAsync */
void fn_801BF8A0(HSD_RenderPass rpass)
{
    int idx;
    void* buffer;

    if (_p->nb_xfb < 2) {
        return;
    }
    idx = HSD_VIWaitXFBDrawEnable();
    buffer = _p->xfb[idx].buffer;
    fn_801BFA1C(&_p->current.vi, buffer, rpass);

    while (fn_801BFCA0()) {
        fn_800B8DA8();
    }
    _p->drawdone.waiting = 1;
    _p->drawdone.arg = idx;
    GXDrawDone();
}

/* HSD_VICopyEFB2XFBPtr */
void fn_801BFA1C(HSD_VIStatus* vi, void* buffer, HSD_RenderPass rpass)
{
    GXRenderModeObj* rmode = &vi->rmode;
    int n_xfb_lines;
    int rest;
    u16 lines;
    u16 pitch;
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
        n_xfb_lines =
            fn_800B9B14(GXGetYScaleFactor(rmode->efbHeight,
                                          rmode->xfbHeight));
        fn_800B96BC(rmode->fbWidth, n_xfb_lines);
        fn_800B9E88(buffer, TRUE);
        /*
         * Blank the XFB lines the copy did not reach.  `rest` is signed but
         * cannot go negative: GXGetYScaleFactor walks its scale down until
         * __GXGetNumXfbLines stops exceeding xfbHeight, so n_xfb_lines <=
         * xfbHeight and the unsigned conversion in `rest * bpl` below is
         * safe.  Retail tests equality here, not `> 0` -- the target is
         * `subf.` + `beq` at 0x801BFB04, where `> 0` would need `ble`.
         */
        rest = rmode->xfbHeight - n_xfb_lines;
        if (rest != 0) {
            u32 bpl;
            u32 nb;
            u32* dst;

            pitch = (rmode->fbWidth + 15) & ~15;
            bpl = pitch * 2;
            dst = (u32*) ((u8*) buffer + bpl * n_xfb_lines);
            nb = (rest * bpl) / 4;
            while (nb-- != 0) {
                *dst++ = HSD_VI_XFB_BLACK2;
            }
        }
        break;
    case HSD_RP_TOPHALF:
        fn_800B959C(0, 0, rmode->fbWidth,
                    rmode->efbHeight - HSD_ANTIALIAS_OVERLAP);
        n_xfb_lines = fn_800B9B14(lbl_8047DF38[0]);
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
        n_xfb_lines = fn_800B9B14(lbl_8047DF38[0]);
        fn_800B96BC(rmode->fbWidth, n_xfb_lines);
        fn_800B9874(2);
        lines = rmode->efbHeight - HSD_ANTIALIAS_OVERLAP;
        fn_800B959C(0, HSD_ANTIALIAS_OVERLAP, rmode->fbWidth, lines);
        pitch = (rmode->fbWidth + 15) & ~15;
        offset = pitch * lines * 2;
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
