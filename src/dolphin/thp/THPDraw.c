/**
 * @file THPDraw.c
 * @brief Dolphin SDK THP player sample: YUV -> RGB GX drawing,
 *        0x801E1E1C - 0x801E25C8.
 *
 * Built with -inline noauto,deferred (functions are emitted in reverse source
 * order). The GX helpers below are the retail GX entry points that this
 * project has not named yet; their SDK names are given alongside.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/mtx.h"

typedef struct GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef union PPCWGPipe {
    u8 u8;
    s8 s8;
    u16 u16;
    s16 s16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe*)0xCC008000)

extern void GXSetZMode(u32 compareEnable, u32 func, u32 updateEnable);
extern void GXSetBlendMode(u32 type, u32 srcFactor, u32 dstFactor, u32 op);
extern void GXInvalidateTexAll(void);
extern void GXLoadTexObj(GXTexObj* obj, u32 id);
extern void GXLoadPosMtxImm(Mtx mtx, u32 id);
extern void PSMTXIdentity(Mtx mtx);
extern void C_MTXOrtho(f32 mtx[4][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);

extern void GXSetVtxDesc(u32 attr, u32 type);                       /* GXSetVtxDesc */
extern void GXClearVtxDesc(void);                                     /* GXClearVtxDesc */
extern void GXSetVtxAttrFmt(u32 fmt, u32 attr, u32 cnt, u32 type, u8 frac); /* GXSetVtxAttrFmt */
extern void GXSetTexCoordGen2(u32 dst, u32 func, u32 src, u32 mtx, u8 normalize,
                        u32 postMtx);                              /* GXSetTexCoordGen2 */
extern void GXSetNumTexGens(u8 nTexGens);                              /* GXSetNumTexGens */
extern void GXBegin(u8 type, u8 fmt, u16 nverts);              /* GXBegin */
extern void GXSetDispCopyGamma(u8 nChans);                                /* GXSetDispCopyGamma */
extern void GXSetNumChans(u8 nChans);                                /* GXSetNumChans */
extern void GXInitTexObj(GXTexObj* obj, void* image, u16 width, u16 height,
                        u32 format, u32 wrapS, u32 wrapT, u8 mipmap); /* GXInitTexObj */
extern void GXInitTexObjLOD(GXTexObj* obj, u32 minFilt, u32 magFilt, f32 minLOD,
                        f32 maxLOD, f32 lodBias, u8 biasClamp, u8 doEdgeLOD,
                        u32 maxAniso);                             /* GXInitTexObjLOD */
extern void GXSetTevOp(u32 stage, u32 mode);
extern void GXSetTevColorIn(u32 stage, u32 a, u32 b, u32 c, u32 d);    /* GXSetTevColorIn */
extern void GXSetTevAlphaIn(u32 stage, u32 a, u32 b, u32 c, u32 d);    /* GXSetTevAlphaIn */
extern void GXSetTevColorOp(u32 stage, u32 op, u32 bias, u32 scale, u8 clamp,
                        u32 outReg);                               /* GXSetTevColorOp */
extern void GXSetTevAlphaOp(u32 stage, u32 op, u32 bias, u32 scale, u8 clamp,
                        u32 outReg);                               /* GXSetTevAlphaOp */
extern void GXSetTevColorS10(u32 id, GXColorS10 color);                 /* GXSetTevColorS10 */
extern void GXSetTevKColor(u32 id, GXColor color);                    /* GXSetTevKColor */
extern void GXSetTevKColorSel(u32 stage, u32 sel);                       /* GXSetTevKColorSel */
extern void GXSetTevKAlphaSel(u32 stage, u32 sel);                       /* GXSetTevKAlphaSel */
extern void GXSetTevSwapMode(u32 stage, u32 rasSel, u32 texSel);        /* GXSetTevSwapMode */
extern void GXSetTevSwapModeTable(u32 table, u32 red, u32 green, u32 blue, u32 alpha); /* GXSetTevSwapModeTable */
extern void GXSetTevOrder(u32 stage, u32 coord, u32 map, u32 color); /* GXSetTevOrder */
extern void GXSetNumTevStages(u8 nStages);                               /* GXSetNumTevStages */
extern void GXSetColorUpdate(u8 enable);                                /* GXSetColorUpdate */
extern void GXSetAlphaUpdate(u8 enable);                                /* GXSetAlphaUpdate */
extern void GXSetPixelFmt(u32 pixFmt, u32 zFmt);                     /* GXSetPixelFmt */
extern void GXSetProjection(f32 mtx[4][4], u32 type);                  /* GXSetProjection */
extern void GXSetCurrentMtx(u32 id);                                   /* GXSetCurrentMtx */
extern void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz); /* GXSetViewport */
extern void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);        /* GXSetScissor */

void THPGXRestore(void)
{
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0xF);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
}

void THPGXYuv2RgbSetup(GXRenderModeObj* rmode)
{
    s32 w;
    s32 h;
    f32 m[4][4];
    Mtx e_m;

    w = rmode->fbWidth;
    h = rmode->efbHeight;

    GXSetPixelFmt(0, 0);

    C_MTXOrtho(m, 0.0f, h, 0.0f, w, 0.0f, -1.0f);
    GXSetProjection(m, 1);
    GXSetViewport(0.0f, 0.0f, w, h, 0.0f, 1.0f);
    GXSetScissor(0, 0, w, h);

    PSMTXIdentity(e_m);
    GXLoadPosMtxImm(e_m, 0);
    GXSetCurrentMtx(0);

    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(0);
    GXSetDispCopyGamma(0);
    GXSetNumChans(0);

    GXSetNumTexGens(2);
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetTexCoordGen2(1, 1, 4, 60, 0, 125);
    GXInvalidateTexAll();

    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxAttrFmt(7, 9, 1, 3, 0);
    GXSetVtxAttrFmt(7, 13, 1, 2, 0);

    GXSetNumTevStages(4);
    GXSetTevOrder(0, 1, 1, 0xFF);
    GXSetTevColorIn(0, 15, 8, 14, 2);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaIn(0, 7, 4, 6, 1);
    GXSetTevAlphaOp(0, 1, 0, 0, 0, 0);
    GXSetTevKColorSel(0, 12);
    GXSetTevKAlphaSel(0, 28);
    GXSetTevSwapMode(0, 0, 0);

    GXSetTevOrder(1, 1, 2, 0xFF);
    GXSetTevColorIn(1, 15, 8, 14, 0);
    GXSetTevColorOp(1, 0, 0, 1, 0, 0);
    GXSetTevAlphaIn(1, 7, 4, 6, 0);
    GXSetTevAlphaOp(1, 1, 0, 0, 0, 0);
    GXSetTevKColorSel(1, 13);
    GXSetTevKAlphaSel(1, 29);
    GXSetTevSwapMode(1, 0, 0);

    GXSetTevOrder(2, 0, 0, 0xFF);
    GXSetTevColorIn(2, 15, 8, 12, 0);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 4, 7, 7, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevSwapMode(2, 0, 0);

    GXSetTevOrder(3, 0xFF, 0xFF, 0xFF);
    GXSetTevColorIn(3, 1, 0, 14, 15);
    GXSetTevColorOp(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(3, 7, 7, 7, 7);
    GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevKColorSel(3, 14);

    GXSetTevColorS10(1, (GXColorS10){ -90, 0, -114, 135 });
    GXSetTevKColor(0, (GXColor){ 0, 0, 226, 88 });
    GXSetTevKColor(1, (GXColor){ 179, 0, 0, 182 });
    GXSetTevKColor(2, (GXColor){ 255, 0, 255, 128 });
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
}

void THPGXYuv2RgbDraw(u8* y_data, u8* u_data, u8* v_data, s16 x, s16 y,
                 s16 textureWidth, s16 textureHeight, s16 polygonWidth,
                 s16 polygonHeight)
{
    GXTexObj tobj0;
    GXTexObj tobj1;
    GXTexObj tobj2;

    GXInitTexObj(&tobj0, y_data, textureWidth, textureHeight, 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj0, 0, 0, 0, 0, 0, 0, 0, 0);
    GXLoadTexObj(&tobj0, 0);

    GXInitTexObj(&tobj1, u_data, textureWidth >> 1, textureHeight >> 1, 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj1, 0, 0, 0, 0, 0, 0, 0, 0);
    GXLoadTexObj(&tobj1, 1);

    GXInitTexObj(&tobj2, v_data, textureWidth >> 1, textureHeight >> 1, 1, 0, 0, 0);
    GXInitTexObjLOD(&tobj2, 0, 0, 0, 0, 0, 0, 0, 0);
    GXLoadTexObj(&tobj2, 2);

    GXBegin(0x80, 7, 4);
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 1;
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 1;
}
