/**
 * @file hsd_util.c
 * @brief sysdolphin util.c helpers plus the head of video.c.
 *
 * Address range: 0x801BF098 - 0x801BF6AC.
 *
 * Ported from the Melee sysdolphin decompilation
 * (src/sysdolphin/baselib/util.c and src/sysdolphin/baselib/video.c).
 * Colosseum emits both translation units with the functions in the opposite
 * order to Melee's, so the definitions below follow retail address order.
 */

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/vi/VI.h"

extern void __assert(const char* file, u32 line, const char* expr);

/* sdata2 strings owned by hsd_sdata2_8047DE90.c */
extern char lbl_8047DF20; /* "util.c" */
extern char lbl_8047DF28; /* "0"      */

/* ========================================================================= *
 *  util.c
 * ========================================================================= */

/* 0x801BF098 | 0xA0 */
s32 HSD_Index2PosNrmMtx(u32 idx)
{
    switch (idx) {
    case 0:
        return 0;
    case 1:
        return 3;
    case 2:
        return 6;
    case 3:
        return 9;
    case 4:
        return 12;
    case 5:
        return 15;
    case 6:
        return 18;
    case 7:
        return 21;
    case 8:
        return 24;
    case 9:
        return 27;
    default:
        __assert(&lbl_8047DF20, 0x84, &lbl_8047DF28);
        return 0;
    }
}

/* 0x801BF138 | 0x34 */
u32 HSD_GetNbBits(u32 c)
{
    int ret = 0;
    int i;

    for (i = 0; i < 32; i++) {
        if (c & (1 << i)) {
            ret++;
        }
    }
    return ret;
}

/* 0x801BF16C | 0x84 */
void HSD_MulColor(GXColor* a, GXColor* b, GXColor* dest)
{
    dest->r = (a->r * b->r) / 255U;
    dest->g = (a->g * b->g) / 255U;
    dest->b = (a->b * b->b) / 255U;
    dest->a = (a->a * b->a) / 255U;
}

/* ========================================================================= *
 *  video.c (head)
 * ========================================================================= */

#define HSD_VI_XFB_MAX 3

typedef void (*HSD_VIGXDrawDoneCallback)(int);

typedef enum _HSD_VIXFBDrawDispStatus {
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

typedef enum _HSD_VIEFBDrawDispStatus {
    HSD_VI_EFB_FREE,
    HSD_VI_EFB_DRAWDONE,
    HSD_VI_EFB_TERMINATE
} HSD_VIEFBDrawDispStatus;

typedef enum _HSD_RenderPass {
    HSD_RP_SCREEN,
    HSD_RP_TOPHALF,
    HSD_RP_BOTTOMHALF,
    HSD_RP_OFFSCREEN,
    HSD_RP_NUM
} HSD_RenderPass;

typedef struct _HSD_VIStatus {
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

typedef struct _current {
    HSD_VIStatus vi;
    u8 chg_flag;
} Current;

typedef struct _XFB {
    void* buffer;
    HSD_VIXFBDrawDispStatus status;
    Current vi_all;
} XFB;

typedef struct _HSD_VIInfo {
    Current current;

    XFB xfb[HSD_VI_XFB_MAX];

    struct _EFB {
        HSD_VIEFBDrawDispStatus status;
        Current vi_all;
    } efb;

    s32 nb_xfb;

    VIRetraceCallback pre_cb;
    VIRetraceCallback post_cb;

    struct drawdone {
        s32 waiting;
        s32 arg;
        HSD_VIGXDrawDoneCallback cb;
    } drawdone;

    struct perf {
        s32 frame_period;
        s32 frame_renew;
    } perf;
} HSD_VIInfo;

extern HSD_VIInfo lbl_80466BC0;

#define _p ((HSD_VIInfo*) &lbl_80466BC0)

void VIInit(void);
void VIConfigure(GXRenderModeObj* rmode);
void VISetBlack(BOOL black);
void VIFlush(void);
void fn_800B90A4(void (*cb)(void)); /* GXSetDrawDoneCallback */

/* video.c, defined later in the hsd_video unit */
void fn_801BFF18(u32 retraceCount); /* HSD_VIPreRetraceCB  */
void fn_801BFD10(u32 retraceCount); /* HSD_VIPostRetraceCB */
void fn_801BFCB0(void);             /* HSD_VIGXDrawDoneCB  */
void fn_801BFA1C(HSD_VIStatus* vi, void* buffer, HSD_RenderPass rpass);

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

/* 0x801BF1F0 | 0x2D4 -- HSD_VIInit */
void fn_801BF1F0(HSD_VIStatus* vi, void* xfb0, void* xfb1, void* xfb2)
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

    fn_800A880C(fn_801BFF18);
    fn_800A8850(fn_801BFD10);

    _p->pre_cb = NULL;
    _p->post_cb = NULL;

    _p->drawdone.waiting = 0;
    _p->drawdone.arg = 0;

    fn_800B90A4(fn_801BFCB0);
    _p->drawdone.cb = NULL;

    _p->perf.frame_period = VIGetTvFormat() == VI_NTSC ? 60 : 50;
    _p->perf.frame_renew = 0;

    VIConfigure(&_p->current.vi.rmode);
    VISetBlack(_p->current.vi.black);
    VIFlush();

    idx = HSD_VISearchXFBByStatus(HSD_VI_XFB_FREE);
    fn_801BFA1C(&_p->current.vi, _p->xfb[idx].buffer, HSD_RP_SCREEN);
}

/* 0x801BF4C4 | 0x20 -- HSD_VISetBlack */
void fn_801BF4C4(BOOL black)
{
    _p->current.vi.black = black;
    _p->current.chg_flag = 1;
}

/* 0x801BF4E4 | 0x90 -- HSD_VISetConfigure */
void fn_801BF4E4(GXRenderModeObj* rmode)
{
    _p->current.vi.rmode = *rmode;
    _p->current.chg_flag = 1;
}

/* 0x801BF574 | 0x138 -- HSD_VIGetXFBLastDrawDone */
int fn_801BF574(void)
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
