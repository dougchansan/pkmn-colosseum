/**
 * @file gs_gfx_layer.h
 * @brief GSgfx layer state: the channel / texgen / TEV block that the
 * GSgfx layer setters fill and _gfxFlushTevState flushes to GX.
 *
 * Partial view of the state behind lbl_8047AA80. Every offset is taken
 * from the target's loads and stores (_gfxFlushTevState, fn_800D963C,
 * fn_800D892C); fields no carve touches are left as padding. It is a
 * separate view from gs_gfx.h's older GSgfxState, whose layout in this
 * range is still unverified.
 */
#ifndef GS_GFX_LAYER_H
#define GS_GFX_LAYER_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

typedef struct GSChanCtrl {
    u8 enable;
    u8 ambSrc;
    u8 matSrc;
    u8 lightMask;
    u8 diffFn;
    u8 attnFn;
} GSChanCtrl;

typedef struct GSTevOrder {
    u8 coord;
    u8 map;
    u8 color;
} GSTevOrder;

typedef struct GSTevOp {
    u8 op;
    u8 bias;
    u8 scale;
    u8 clamp;
    u8 out;
} GSTevOp;

typedef struct GSTevIn {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
} GSTevIn;

typedef struct GSTevInd {
    u8 type;
    u8 indStage;
    u8 mtxSel;
    u8 format;
    u8 biasSel;
    u8 alphaSel;
    u8 wrapS;
    u8 wrapT;
    u8 addPrev;
    u8 utcLod;
    u8 warpSigned;
    u8 warpReplace;
    u16 tileSizeS;
    u16 tileSizeT;
    u16 tileSpacingS;
    u16 tileSpacingT;
} GSTevInd;

typedef struct GSIndOrder {
    u8 coord;
    u8 map;
    u8 scaleS;
    u8 scaleT;
} GSIndOrder;

typedef struct GSIndMtx {
    f32 mtx[2][3];
    u8 scale;
} GSIndMtx;

/*
 * The GX channel / texgen / TEV block. It lives at +0x60 in the GSgfx
 * state, and a second copy (lbl_80400B28) holds a saved block that
 * fn_800D892C loads back field by field; offsets below are relative to
 * the block, with the GSgfx-state offset in brackets.
 */
typedef struct GSgfxTevState {
    u8 numChans;                 /* 0x000 [0x060] */
    GSChanCtrl chanCtrl[4];      /* 0x001 [0x061]: colour0, alpha0, colour1, alpha1 */
    u8 numTexGens;               /* 0x019 [0x079] */
    u8 numTevStages;             /* 0x01A [0x07A] */
    GSTevOrder tevOrder[16];     /* 0x01B [0x07B] */
    GSTevOp colorOp[16];         /* 0x04B [0x0AB] */
    GSTevOp alphaOp[16];         /* 0x09B [0x0FB] */
    GSTevIn colorIn[16];         /* 0x0EB [0x14B] */
    GSTevIn alphaIn[16];         /* 0x12B [0x18B] */
    u32 kcolorSel[16];           /* 0x16C [0x1CC] */
    u32 kalphaSel[16];           /* 0x1AC [0x20C] */
    GXColor kcolor[4];           /* 0x1EC [0x24C] */
    u8 indEnable[16];            /* 0x1FC [0x25C] */
    GSTevInd ind[16];            /* 0x20C [0x26C] */
    u8 numIndStages;             /* 0x34C [0x3AC] */
    GSIndOrder indOrder[4];      /* 0x34D [0x3AD] */
    GSIndMtx indMtx[3];          /* 0x360 [0x3C0] */
} GSgfxTevState;

/* Per-stage sources: indices into the colour, texcoord and texmap tables,
 * and the TEV preset mode for fn_800D963C. */
typedef struct GSTevStageSrc {
    u8 color;
    u8 coord;
    u8 map;
    u8 mode;
} GSTevStageSrc;

/* One vertex attribute of a GSgfx vertex descriptor (see gfxVtxDescSetAttr). */
typedef struct GSVtxAttr {
    u8 enabled;
    u32 type;
    u32 count;
    u32 compType;
    u8 frac;
    void* data;
    u8 stride;
} GSVtxAttr;

/* GSgfx vertex descriptor: attribute 0 matrix index, 1 position,
 * 2 normal, 4-5 colours, 6-13 texture coordinates. */
typedef struct GSVtxDesc {
    u8 active;
    u32 handle;
    GSVtxAttr attr[14];
} GSVtxDesc;

/* One display-list capture slot (GSgfxDLBegin / GSgfxDLEnd). */
typedef struct GSgfxDLCapture {
    u8 active;
    u8 overflow;
    u16 handle;
    void* data;
    u32 size;
    GSVtxDesc* desc;
    u32 totalVerts;
    u32 totalPrims;
} GSgfxDLCapture;

typedef void (*GSVtxEmitFn)(u32 index);

/* Last value written for a three-component attribute, kept in every
 * component format the emitters use. */
typedef struct GSVtxValue3 {
    u8 b[3];
    u16 s[3];
    f32 f[3];
} GSVtxValue3;

/* Last colour written, as RGBA8 bytes, packed 16-bit and packed 32-bit. */
typedef struct GSVtxColor {
    u8 b[4];
    u16 s;
    u32 w;
} GSVtxColor;

/* The same for a two-component texture coordinate. */
typedef struct GSVtxValue2 {
    u8 b[2];
    u16 s[2];
    f32 f[2];
} GSVtxValue2;

typedef struct GSgfxLayerState {
    s32 recordMode;              /* 0x000 */
    u32 drawMask;                /* 0x004 */
    u32 drawEnable;              /* 0x008 */
    u8 pad_00C[0x10 - 0xC];
    u32 flags;                   /* 0x010 */
    s32 primType;                /* 0x014 */
    u8 lineHalf;                 /* 0x018 */
    u8 pad_019;
    u8 layerCur;                 /* 0x01A */
    u8 layerDraw;                /* 0x01B */
    u8 pad_01C[0x24 - 0x1C];
    GSVtxDesc* vtxDesc;          /* 0x024 */
    u8 pad_028[0x60 - 0x28];
    GSgfxTevState tev;           /* 0x060 */
    u32 dirty;                   /* 0x414: 1 channels, 2 texgens, 4 TEV */
    u8 pad_418[0x42E - 0x418];
    GSTevStageSrc stageSrc[16];  /* 0x42E */
    u8 pad_46E[0x47E - 0x46E];
    u8 captureActive;            /* 0x47E */
    u8 pad_47F;
    GSgfxDLCapture* captureDL;   /* 0x480 */
    u8* captureCursor;           /* 0x484 */
    s32 capturePrimType;         /* 0x488 */
    u8 pad_48C[0x49F - 0x48C];
    u8 vtxPending;               /* 0x49F */
    GSVtxEmitFn emitMtxIdx;      /* 0x4A0 */
    u8 pad_4A4[4];
    GSVtxEmitFn emitPos;         /* 0x4A8 */
    GSVtxValue3 pos;             /* 0x4AC */
    GSVtxEmitFn emitNrm;         /* 0x4C4 */
    GSVtxValue3 nrm;             /* 0x4C8 */
    GSVtxEmitFn emitClr[2];      /* 0x4E0 */
    GSVtxColor clr[2];           /* 0x4E8 */
    GSVtxEmitFn emitTex[8];      /* 0x500 */
    GSVtxValue2 tex[8];          /* 0x520 */
} GSgfxLayerState;

/* Per-frame GX state upload counters. */
typedef struct GSgfxStats {
    u32 pad_00[6];
    u32 chanUpdates;             /* 0x18 */
    u32 texGenUpdates;           /* 0x1C */
    u32 tevUpdates;              /* 0x20 */
} GSgfxStats;

extern GSgfxLayerState* lbl_8047AA80;
extern GSgfxStats lbl_804001F0;

/*
 * GX-style setters for the block above. None exists out of line; each is
 * expanded at every use (four TEV setters per case in fn_800D963C's five
 * presets, the channel and TEV setters again throughout fn_800D892C), and
 * each ends by setting its dirty bit.
 */
static inline void GSgfxSetNumChans(u8 num) {
    lbl_8047AA80->tev.numChans = num;
    lbl_8047AA80->dirty |= 1;
}

/* chan follows GXChannelID: 4 and 5 (GX_COLOR0A0 / GX_COLOR1A1) set a
 * colour channel and the entry after it. */
static inline void GSgfxSetChanCtrl(s32 chan, u8 enable, u8 ambSrc, u8 matSrc, u8 lightMask, u8 diffFn, u8 attnFn) {
    u8 both = FALSE;
    u8 done;
    GSChanCtrl* ctrl;

    if (chan == 4) {
        chan = 0;
        both = TRUE;
    } else if (chan == 5) {
        chan = 1;
        both = TRUE;
    }
    done = FALSE;
    while (!done) {
        ctrl = &lbl_8047AA80->tev.chanCtrl[chan];
        ctrl->enable = enable;
        ctrl->ambSrc = ambSrc;
        ctrl->matSrc = matSrc;
        ctrl->lightMask = lightMask;
        ctrl->diffFn = diffFn;
        ctrl->attnFn = attnFn;
        if (both) {
            both = FALSE;
            chan++;
        } else {
            done = TRUE;
        }
    }
    lbl_8047AA80->dirty |= 1;
}

static inline void GSgfxSetNumTexGens(u8 num) {
    lbl_8047AA80->tev.numTexGens = num;
    lbl_8047AA80->dirty |= 2;
}

static inline void GSgfxSetNumTevStages(u8 num) {
    lbl_8047AA80->tev.numTevStages = num;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevOrder(u32 stage, s32 coord, s32 map, s32 color) {
    GSTevOrder* order = &lbl_8047AA80->tev.tevOrder[stage];

    order->coord = coord;
    order->map = map;
    order->color = color;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetNumIndStages(u8 num) {
    lbl_8047AA80->tev.numIndStages = num;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevColorOp(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out) {
    GSTevOp* tevOp = &lbl_8047AA80->tev.colorOp[stage];

    tevOp->op = op;
    tevOp->bias = bias;
    tevOp->scale = scale;
    tevOp->clamp = clamp;
    tevOp->out = out;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevAlphaOp(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out) {
    GSTevOp* tevOp = &lbl_8047AA80->tev.alphaOp[stage];

    tevOp->op = op;
    tevOp->bias = bias;
    tevOp->scale = scale;
    tevOp->clamp = clamp;
    tevOp->out = out;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevColorIn(u32 stage, u8 a, u8 b, u8 c, u8 d) {
    GSTevIn* tevIn = &lbl_8047AA80->tev.colorIn[stage];

    tevIn->a = a;
    tevIn->b = b;
    tevIn->c = c;
    tevIn->d = d;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevAlphaIn(u32 stage, u8 a, u8 b, u8 c, u8 d) {
    GSTevIn* tevIn = &lbl_8047AA80->tev.alphaIn[stage];

    tevIn->a = a;
    tevIn->b = b;
    tevIn->c = c;
    tevIn->d = d;
    lbl_8047AA80->dirty |= 4;
}

#endif /* GS_GFX_LAYER_H */
