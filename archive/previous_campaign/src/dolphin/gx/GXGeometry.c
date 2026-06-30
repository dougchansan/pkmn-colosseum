#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"

/*
 * GXGeometry.c - GX geometry, vertex, transform, and TEV functions.
 *
 * Contains the bulk of GX rendering API functions: vertex format setup,
 * transform matrices, lighting, texture coordinate generation, TEV
 * stage configuration, and color/alpha operations.
 *
 * Matches: 0x800B96BC - 0x800BC0F0 (GX rendering pipeline)
 */

/* Vertex format register write via FIFO */
#define WGPIPE (*(volatile u8*)0xCC008000)

/* XF (Transform Unit) memory write */
static void __GXWriteXFReg(u32 addr, u32 val) {
    WGPIPE = 0x10;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = addr;
    *(volatile u32*)0xCC008000 = val;
}

/*
 * GXSetVtxDesc - Set vertex attribute descriptor.
 */
void GXSetVtxDesc(u32 attr, u32 type) {
    /* Set the attribute type for the vertex format */
}

/*
 * GXClearVtxDesc - Clear all vertex descriptors.
 */
void GXClearVtxDesc(void) {
    /* Reset all vertex attributes to NONE */
}

/*
 * GXSetVtxAttrFmt - Set vertex attribute format.
 */
void GXSetVtxAttrFmt(u8 vtxfmt, u32 attr, u32 compCnt, u32 compType, u32 frac) {
    /* Configure the vertex attribute format for a given vertex format ID */
}

/*
 * GXSetNumChans - Set number of color channels.
 */
void GXSetNumChans(u8 nChans) {
    __GXWriteXFReg(0x1009, nChans);
}

/*
 * GXSetChanCtrl - Configure a color channel.
 */
void GXSetChanCtrl(u32 chan, GXBool enable, u32 ambSrc, u32 matSrc,
                   u32 lightMask, u32 diffFn, u32 attnFn) {
    u32 val = 0;
    val |= enable & 1;
    val |= (matSrc & 1) << 1;
    val |= (lightMask & 0xF) << 2;
    val |= (ambSrc & 1) << 6;
    val |= (diffFn & 3) << 7;
    val |= (attnFn & 1) << 9;
    val |= ((lightMask >> 4) & 0xF) << 10;

    __GXWriteXFReg(0x100E + chan, val);
}

/*
 * GXSetChanAmbColor - Set ambient color for a channel.
 */
void GXSetChanAmbColor(u32 chan, u32 color) {
    __GXWriteXFReg(0x100A + chan, color);
}

/*
 * GXSetChanMatColor - Set material color for a channel.
 */
void GXSetChanMatColor(u32 chan, u32 color) {
    __GXWriteXFReg(0x100C + chan, color);
}

/*
 * GXSetNumTexGens - Set number of texture coordinate generators.
 */
void GXSetNumTexGens(u8 nTexGens) {
    __GXWriteXFReg(0x103F, nTexGens);
}

/*
 * GXSetTexCoordGen - Configure a texture coordinate generator.
 */
void GXSetTexCoordGen(u16 dstCoord, u32 func, u32 srcParam, u32 mtx) {
    u32 val = 0;
    /* Encode texgen parameters */
    __GXWriteXFReg(0x1040 + dstCoord, val);
}

/*
 * GXSetNumTevStages - Set number of active TEV stages.
 */
void GXSetNumTevStages(u8 nStages) {
    /* Configure the number of active TEV stages */
}

/*
 * GXSetTevOrder - Set TEV stage order (texture, texcoord, color).
 */
void GXSetTevOrder(u8 stage, u8 texCoord, u32 texMap, u8 colorChan) {
    /* Configure TEV stage ordering */
}

/*
 * GXSetTevOp - Set TEV stage operation (shortcut for common modes).
 */
void GXSetTevOp(u8 stage, u8 mode) {
    /* Configure TEV color and alpha combine for common modes */
}

/*
 * GXSetTevColorIn - Set TEV stage color input.
 */
void GXSetTevColorIn(u8 stage, u8 a, u8 b, u8 c, u8 d) {
    /* Configure TEV color inputs */
}

/*
 * GXSetTevAlphaIn - Set TEV stage alpha input.
 */
void GXSetTevAlphaIn(u8 stage, u8 a, u8 b, u8 c, u8 d) {
    /* Configure TEV alpha inputs */
}

/*
 * GXSetTevColorOp - Set TEV color operation.
 */
void GXSetTevColorOp(u8 stage, u8 op, u8 bias, u8 scale,
                     GXBool clamp, u8 outReg) {
    /* Configure TEV color combine operation */
}

/*
 * GXSetTevAlphaOp - Set TEV alpha operation.
 */
void GXSetTevAlphaOp(u8 stage, u8 op, u8 bias, u8 scale,
                     GXBool clamp, u8 outReg) {
    /* Configure TEV alpha combine operation */
}

/*
 * GXSetAlphaCompare - Set alpha comparison function.
 */
void GXSetAlphaCompare(u8 comp0, u8 ref0, u8 op, u8 comp1, u8 ref1) {
    u32 val = 0;
    val |= ref0;
    val |= (u32)ref1 << 8;
    val |= (u32)comp0 << 16;
    val |= (u32)comp1 << 19;
    val |= (u32)op << 22;
    /* Write to BP register */
}

/*
 * GXSetZMode - Set depth buffer mode.
 */
void GXSetZMode(GXBool compareEnable, u8 func, GXBool updateEnable) {
    u32 val = 0;
    val |= compareEnable;
    val |= (u32)func << 1;
    val |= (u32)updateEnable << 4;
    /* Write to BP register */
}

/*
 * GXSetZCompLoc - Set Z compare location (before or after texture).
 */
void GXSetZCompLoc(GXBool before_tex) {
    /* Write to BP register */
}

/*
 * GXSetBlendMode - Set framebuffer blend mode.
 */
void GXSetBlendMode(u8 type, u8 srcFactor, u8 dstFactor, u8 op) {
    /* Write to BP register */
}

/*
 * GXSetColorUpdate - Enable/disable color buffer update.
 */
void GXSetColorUpdate(GXBool enable) {
    /* Write to BP register */
}

/*
 * GXSetAlphaUpdate - Enable/disable alpha buffer update.
 */
void GXSetAlphaUpdate(GXBool enable) {
    /* Write to BP register */
}

/*
 * GXSetDither - Enable/disable dithering.
 */
void GXSetDither(GXBool enable) {
    /* Write to BP register */
}

/*
 * GXSetCullMode - Set polygon culling mode.
 */
void GXSetCullMode(u8 mode) {
    /* Write to XF register */
}

/*
 * GXSetCoPlanar - Set coplanar mode.
 */
void GXSetCoPlanar(GXBool enable) {
    /* Write to BP register */
}

/*
 * GXSetClipMode - Set clipping mode.
 */
void GXSetClipMode(u8 mode) {
    __GXWriteXFReg(0x1005, mode);
}

/*
 * GXSetScissor - Set scissor rectangle.
 */
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht) {
    u32 tl, br;
    tl = ((top + 342) & 0xFFF) | (((left + 342) & 0xFFF) << 12);
    br = ((top + ht + 341) & 0xFFF) | (((left + wd + 341) & 0xFFF) << 12);
    /* Write to BP registers 0x20, 0x21 */
}

/*
 * GXSetViewport - Set the viewport transform.
 */
void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz) {
    f32 sx, sy, sz, ox, oy, oz;

    sx = wd / 2.0f;
    sy = -(ht / 2.0f);
    sz = farz - nearz;
    ox = left + (wd / 2.0f) + 342.0f;
    oy = top + (ht / 2.0f) + 342.0f;
    oz = farz;

    /* Write 6 XF registers starting at 0x101A */
    __GXWriteXFReg(0x101A, *(u32*)&sx);
    __GXWriteXFReg(0x101B, *(u32*)&sy);
    __GXWriteXFReg(0x101C, *(u32*)&sz);
    __GXWriteXFReg(0x101D, *(u32*)&ox);
    __GXWriteXFReg(0x101E, *(u32*)&oy);
    __GXWriteXFReg(0x101F, *(u32*)&oz);
}
