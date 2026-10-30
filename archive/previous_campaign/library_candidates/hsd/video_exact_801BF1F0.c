/**
 * @file video_exact_801BF1F0.c
 * @brief HAL video.c, head of the retail text: HSD_VIInit through
 *        HSD_VIGetXFBLastDrawDone, 0x801BF1F0-0x801BF6AC.
 *
 * Text-only carve of src/hsd/video.c, the full HAL translation unit, which
 * stays the reference and the candidate for 0x801BF6AC-0x801BFA1C. There,
 * HSD_VICopyXFBAsync (fn_801BF8A0) calls HSD_VIGetDrawDoneWaitingFlag out
 * of line; Melee reproduces that only with a local dont_inline pragma,
 * which campaign policy forbids, so video.c cannot link as one unit.
 * HSD_VIFinishDrawXFB (exact in the full TU) stays with it: its asserts address
 * the TU's string pool through one hoisted base, which a carved unit
 * cannot reproduce without owning the pool that the tail's
 * HSD_VIPreRetraceCB also uses. The exact ranges on either side are linked
 * as text-only units built with the same library flags; video.c's data
 * stays in its data units. Functions are listed in HAL's order; MWCC emits
 * them in reverse.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OSInterrupt.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/video.h"

void VIInit(void);
void VIConfigure(GXRenderModeObj* rm);
void VIFlush(void);
void VISetBlack(BOOL black);
u32 VIGetTvFormat(void);
HSD_VIRetraceCallback VISetPreRetraceCallback(HSD_VIRetraceCallback cb); /* VISetPreRetraceCallback */
HSD_VIRetraceCallback VISetPostRetraceCallback(HSD_VIRetraceCallback cb); /* VISetPostRetraceCallback */
void GXSetDrawDoneCallback(void (*cb)(void));             /* GXSetDrawDoneCallback */

void HSD_VIPreRetraceCB(u32 retraceCount);             /* HSD_VIPreRetraceCB */
void HSD_VIPostRetraceCB(u32 retraceCount);             /* HSD_VIPostRetraceCB */
void HSD_VIGXDrawDoneCB(void);                         /* HSD_VIGXDrawDoneCB */

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
