/**
 * @file video.h
 * @brief HAL sysdolphin video.h: VI/XFB state and its inline accessors.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/video.h). Colosseum's layout is the same
 * (HSD_VIInfo is 0x1F4 bytes; its .bss slot is padded to 0x1F8).
 */
#ifndef SYSDOLPHIN_BASELIB_VIDEO_H
#define SYSDOLPHIN_BASELIB_VIDEO_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

#define HSD_VI_XFB_MAX 3
#define HSD_ANTIALIAS_OVERLAP 4
#define VI_DISPLAY_PIX_SZ 2
#define HSD_ANTIALIAS_GARBAGE_SIZE \
    (640 * HSD_ANTIALIAS_OVERLAP * VI_DISPLAY_PIX_SZ)

#ifndef VIPadFrameBufferWidth
#define VIPadFrameBufferWidth(width) ((u16) (((u16) (width) + 15) & ~15))
#endif

typedef void (*HSD_VIGXDrawDoneCallback)(int);
typedef void (*HSD_VIRetraceCallback)(u32);

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

#ifndef HSD_INITIALIZE_H
typedef enum _HSD_RenderPass {
    HSD_RP_SCREEN,
    HSD_RP_TOPHALF,
    HSD_RP_BOTTOMHALF,
    HSD_RP_OFFSCREEN,
    HSD_RP_NUM
} HSD_RenderPass;
#endif

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
    struct _HSD_VIStatus vi;
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

    HSD_VIRetraceCallback pre_cb;
    HSD_VIRetraceCallback post_cb;

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

/* HAL: HSD_VIData */
extern HSD_VIInfo lbl_80466BC0;

/* dtk names; HAL names in the comments */
void HSD_VIInit(HSD_VIStatus* vi, void* xfb0, void* xfb1,
                 void* xfb2);                   /* HSD_VIInit */
void HSD_VISetBlack(BOOL black);                   /* HSD_VISetBlack */
void HSD_VISetConfigure(GXRenderModeObj* rmode);       /* HSD_VISetConfigure */
int HSD_VIGetXFBLastDrawDone(void);                          /* HSD_VIGetXFBLastDrawDone */
void fn_801BF8A0(HSD_RenderPass rpass);         /* HSD_VICopyXFBAsync */
void HSD_VICopyEFB2XFB(HSD_VIStatus* vi, void* buffer,
                 HSD_RenderPass rpass);         /* HSD_VICopyEFB2XFBPtr */
HSD_VIGXDrawDoneCallback
HSD_VISetUserGXDrawDoneCallback(HSD_VIGXDrawDoneCallback cb);       /* HSD_VISetUserGXDrawDoneCallback */
HSD_VIRetraceCallback
HSD_VISetUserPostRetraceCallback(HSD_VIRetraceCallback cb);          /* HSD_VISetUserPostRetraceCallback */

static inline int HSD_VIGetNbXFB(void)
{
    return lbl_80466BC0.nb_xfb;
}

static inline void* HSD_VIGetXFBPtr(int idx)
{
    return lbl_80466BC0.xfb[idx].buffer;
}

static inline HSD_VIStatus* HSD_VIGetVIStatus(void)
{
    return &lbl_80466BC0.current.vi;
}

static inline GXRenderModeObj* HSD_VIGetRenderMode(void)
{
    return &lbl_80466BC0.current.vi.rmode;
}

#endif /* SYSDOLPHIN_BASELIB_VIDEO_H */
