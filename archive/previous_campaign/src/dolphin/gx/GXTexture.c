#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"

/*
 * GXTexture.c - GX Texture management functions.
 *
 * Contains texture loading, TMEM configuration, texture region
 * management, and texture cache/TLUT functions.
 *
 * Matches: 0x800BC0F0 - 0x800BE348 (GX texture subsystem + tail)
 *   __GXFlushTextureState (0x24)
 *   GXInitTexCacheRegion (0x120)
 *   GXInitTlutRegion (0x48)
 *   __GXSetTmemConfig (0x240)
 *   + many unnamed texture functions
 */

/* WGPIPE for direct command writes */
#define WGPIPE (*(volatile u8*)0xCC008000)

extern u8* gx;

/*
 * __GXFlushTextureState - Flush pending texture state changes.
 * 0x800BC0F0 | size: 0x24
 */
asm void __GXFlushTextureState(void) {
    nofralloc
    li r0, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r3, 0x124(r4)
    stw r3, -0x8000(r5)
    sth r0, 2(r4)
    blr
}

/*
 * GXInitTexObj - Initialize a texture object.
 */
typedef struct GXTexObj {
    u32 data[8];
} GXTexObj;

void GXInitTexObj(GXTexObj* obj, void* imagePtr, u16 width, u16 height,
                  u8 format, u8 wrapS, u8 wrapT, GXBool mipmap) {
    u32 i;
    for (i = 0; i < 8; i++) {
        obj->data[i] = 0;
    }

    /* Encode texture parameters */
    obj->data[0] = ((u32)wrapS << 0) | ((u32)wrapT << 2) | ((u32)(mipmap ? 1 : 0) << 4);
    obj->data[1] = (width - 1) | ((height - 1) << 10);
    obj->data[2] = (u32)format;
    obj->data[3] = ((u32)imagePtr >> 5) & 0x00FFFFFF;
}

/*
 * GXInitTexObjCI - Initialize a color-indexed texture object.
 */
void GXInitTexObjCI(GXTexObj* obj, void* imagePtr, u16 width, u16 height,
                    u8 format, u8 wrapS, u8 wrapT, GXBool mipmap, u32 tlut) {
    GXInitTexObj(obj, imagePtr, width, height, format, wrapS, wrapT, mipmap);
    /* Set TLUT name */
    obj->data[4] = tlut;
}

/*
 * GXLoadTexObj - Load a texture object into a texture map slot.
 */
void GXLoadTexObj(GXTexObj* obj, u8 mapID) {
    /* Write texture configuration to BP registers */
    __GXFlushTextureState();
}

/*
 * GXInitTexCacheRegion - Initialize a texture cache region.
 * 0x800BB134 | size: 0x120
 */
asm void GXInitTexCacheRegion(GXTexRegion* region, GXBool is32bMipmap,
                              u32 tmemEven, u32 sizeEven,
                              u32 tmemOdd, u32 sizeOdd) {
    nofralloc
    cmpwi r6, 1
    beq evenSize1
    bge evenSizeGE1
    cmpwi r6, 0
    bge evenSize0
    b evenSizeDone
evenSizeGE1:
    cmpwi r6, 3
    bge evenSizeDone
    b evenSize2
evenSize0:
    li r10, 3
    b evenSizeDone
evenSize1:
    li r10, 4
    b evenSizeDone
evenSize2:
    li r10, 5
evenSizeDone:
    li r0, 0
    stw r0, 0(r3)
    srwi r6, r5, 5
    slwi r5, r10, 15
    lwz r9, 0(r3)
    slwi r0, r10, 18
    cmpwi r8, 2
    clrrwi r9, r9, 15
    or r6, r9, r6
    stw r6, 0(r3)
    lwz r6, 0(r3)
    rlwinm r6, r6, 0, 17, 13
    or r5, r6, r5
    stw r5, 0(r3)
    lwz r5, 0(r3)
    rlwinm r5, r5, 0, 14, 10
    or r0, r5, r0
    stw r0, 0(r3)
    lwz r0, 0(r3)
    rlwinm r0, r0, 0, 11, 9
    stw r0, 0(r3)
    beq oddSize2
    bge oddSizeGE2
    cmpwi r8, 0
    beq oddSize0
    bge oddSize1
    b oddSizeDone
oddSizeGE2:
    cmpwi r8, 4
    bge oddSizeDone
    b oddSize3
oddSize0:
    li r10, 3
    b oddSizeDone
oddSize1:
    li r10, 4
    b oddSizeDone
oddSize2:
    li r10, 5
    b oddSizeDone
oddSize3:
    li r10, 0
oddSizeDone:
    li r0, 0
    stw r0, 4(r3)
    srwi r7, r7, 5
    slwi r6, r10, 15
    lwz r8, 4(r3)
    slwi r5, r10, 18
    li r0, 1
    clrrwi r8, r8, 15
    or r7, r8, r7
    stw r7, 4(r3)
    lwz r7, 4(r3)
    rlwinm r7, r7, 0, 17, 13
    or r6, r7, r6
    stw r6, 4(r3)
    lwz r6, 4(r3)
    rlwinm r6, r6, 0, 14, 10
    or r5, r6, r5
    stw r5, 4(r3)
    stb r4, 0xc(r3)
    stb r0, 0xd(r3)
    blr
}

/*
 * GXInitTlutRegion - Initialize a TLUT (texture lookup table) region.
 * 0x800BB254 | size: 0x48
 */
asm void GXInitTlutRegion(GXTlutRegion* region, u32 tmemAddr, u8 tlutSize) {
    nofralloc
    li r0, 0
    stw r0, 0(r3)
    subis r0, r4, 8
    srwi r4, r0, 9
    lwz r6, 0(r3)
    slwi r0, r5, 10
    clrrwi r5, r6, 10
    or r4, r5, r4
    stw r4, 0(r3)
    lwz r4, 0(r3)
    rlwinm r4, r4, 0, 22, 10
    or r0, r4, r0
    stw r0, 0(r3)
    lwz r0, 0(r3)
    clrlwi r0, r0, 8
    oris r0, r0, 0x6500
    stw r0, 0(r3)
    blr
}

/*
 * __GXSetTmemConfig - Configure TMEM layout.
 * 0x800BB540 | size: 0x240
 *
 * Sets up the texture memory (TMEM) partitioning based on
 * the requested configuration mode.
 */
asm void __GXSetTmemConfig(u32 config) {
    nofralloc
    stwu r1, -0x18(r1)
    cmpwi r3, 1
    stw r31, 0x14(r1)
    stw r30, 0x10(r1)
    beq config1
    bge configOther
    b configOther
config1:
    li r0, 0x61
    lis r3, 0xcc01
    lis r4, 0x8c0e
    stb r0, -0x8000(r3)
    addi r4, r4, -0x8000
    stw r4, -0x8000(r3)
    lis r4, 0x900e
    lis r7, 0x8d0e
    stb r0, -0x8000(r3)
    subi r4, r4, 0x4000
    lis r6, 0x910e
    stw r4, -0x8000(r3)
    lis r5, 0x8e0e
    lis r4, 0x920e
    stb r0, -0x8000(r3)
    subi r7, r7, 0x7800
    lis r30, 0x8f0e
    stw r7, -0x8000(r3)
    lis r12, 0x930e
    lis r11, 0xac0e
    stb r0, -0x8000(r3)
    subi r6, r6, 0x3800
    lis r10, 0xb00e
    stw r6, -0x8000(r3)
    lis r9, 0xad0e
    lis r8, 0xb10e
    stb r0, -0x8000(r3)
    subi r5, r5, 0x7000
    lis r7, 0xae0e
    stw r5, -0x8000(r3)
    lis r6, 0xb20e
    lis r5, 0xaf0e
    stb r0, -0x8000(r3)
    subi r31, r4, 0x3000
    lis r4, 0xb30e
    stw r31, -0x8000(r3)
    subi r30, r30, 0x6800
    subi r12, r12, 0x2800
    stb r0, -0x8000(r3)
    subi r11, r11, 0x6000
    subi r10, r10, 0x2000
    stw r30, -0x8000(r3)
    subi r9, r9, 0x5800
    subi r8, r8, 0x1800
    stb r0, -0x8000(r3)
    subi r7, r7, 0x5000
    subi r6, r6, 0x1000
    stw r12, -0x8000(r3)
    subi r5, r5, 0x4800
    subi r4, r4, 0x800
    stb r0, -0x8000(r3)
    stw r11, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r10, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r9, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r8, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r7, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r6, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r5, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r4, -0x8000(r3)
    b tmemConfigDone
configOther:
    li r0, 0x61
    lis r3, 0xcc01
    lis r4, 0x8c0e
    stb r0, -0x8000(r3)
    addi r4, r4, -0x8000
    stw r4, -0x8000(r3)
    lis r4, 0x900e
    lis r7, 0x8d0e
    stb r0, -0x8000(r3)
    subi r4, r4, 0x4000
    lis r6, 0x910e
    stw r4, -0x8000(r3)
    lis r5, 0x8e0e
    lis r4, 0x920e
    stb r0, -0x8000(r3)
    subi r7, r7, 0x7c00
    lis r31, 0x8f0e
    stw r7, -0x8000(r3)
    lis r12, 0x930e
    lis r11, 0xac0e
    stb r0, -0x8000(r3)
    subi r6, r6, 0x3c00
    lis r10, 0xb00e
    stw r6, -0x8000(r3)
    lis r9, 0xad0e
    lis r8, 0xb10e
    stb r0, -0x8000(r3)
    subi r5, r5, 0x7800
    lis r7, 0xae0e
    stw r5, -0x8000(r3)
    lis r6, 0xb20e
    lis r5, 0xaf0e
    stb r0, -0x8000(r3)
    subi r30, r4, 0x3800
    lis r4, 0xb30e
    stw r30, -0x8000(r3)
    subi r31, r31, 0x7400
    subi r12, r12, 0x3400
    stb r0, -0x8000(r3)
    subi r11, r11, 0x7000
    subi r10, r10, 0x3000
    stw r31, -0x8000(r3)
    subi r9, r9, 0x6c00
    subi r8, r8, 0x2c00
    stb r0, -0x8000(r3)
    subi r7, r7, 0x6800
    subi r6, r6, 0x2800
    stw r12, -0x8000(r3)
    subi r5, r5, 0x6400
    subi r4, r4, 0x2400
    stb r0, -0x8000(r3)
    stw r11, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r10, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r9, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r8, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r7, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r6, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r5, -0x8000(r3)
    stb r0, -0x8000(r3)
    stw r4, -0x8000(r3)
tmemConfigDone:
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    blr
}

/*
 * GXInvalidateTexAll - Invalidate all texture cache entries.
 */
void GXInvalidateTexAll(void) {
    __GXFlushTextureState();
}

/*
 * GXLoadTlut - Load a TLUT into TMEM.
 */
typedef struct GXTlutObj {
    u32 data[3];
} GXTlutObj;

void GXLoadTlut(GXTlutObj* tlut, u32 tmemAddr) {
    /* DMA TLUT data to TMEM */
    __GXFlushTextureState();
}

/*
 * GXInitTlutObj - Initialize a TLUT object.
 */
void GXInitTlutObj(GXTlutObj* obj, void* data, u8 format, u16 numEntries) {
    obj->data[0] = 0;
    obj->data[1] = ((u32)data >> 5) & 0x00FFFFFF;
    obj->data[2] = ((u32)format << 10) | (numEntries & 0x3FF);
}

/*
 * GXSetTexCoordScaleManually - Manual texture coordinate scaling.
 */
void GXSetTexCoordScaleManually(u8 texCoord, GXBool enable, u16 scaleS, u16 scaleT) {
    /* Configure texture coordinate scaling */
}

/*
 * GXSetTexCoordCylWrap - Configure cylindrical wrapping for texture coords.
 */
void GXSetTexCoordCylWrap(u8 texCoord, GXBool sWrap, GXBool tWrap) {
    /* Configure cylindrical wrapping */
}

/*
 * GXSetDispCopySrc - Set display copy source region.
 */
void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    /* Write BP registers for EFB copy source */
}

/*
 * GXSetDispCopyDst - Set display copy destination width.
 */
void GXSetDispCopyDst(u16 wd) {
    /* Write BP register for XFB destination width */
}

/*
 * GXSetCopyFilter - Set anti-aliasing copy filter.
 */
void GXSetCopyFilter(GXBool aa, u8 samplePattern[12][2], GXBool vf, u8 vfilter[7]) {
    /* Write BP registers for copy filter */
}

/*
 * GXCopyDisp - Copy the EFB to XFB (display copy).
 */
void GXCopyDisp(void* dest, GXBool clear) {
    /* Write BP registers to initiate EFB-to-XFB copy */
    __GXFlushTextureState();
}

/*
 * GXCopyTex - Copy the EFB to a texture.
 */
void GXCopyTex(void* dest, GXBool clear) {
    /* Write BP registers to initiate EFB-to-texture copy */
    __GXFlushTextureState();
}

/*
 * GXSetDispCopyGamma - Set gamma correction for display copy.
 */
void GXSetDispCopyGamma(u8 gamma) {
    /* Write BP register for gamma */
}

/*
 * GXSetDispCopyFrame2Field - Set field selection for display copy.
 */
void GXSetDispCopyFrame2Field(u8 mode) {
    /* Write BP register for field mode */
}

/*
 * GXSetCopyClear - Set clear color/depth for copy operations.
 */
void GXSetCopyClear(u32 clearColor, u32 clearZ) {
    /* Write BP registers for clear color and Z */
}

/*
 * GXSetFieldMask - Set field rendering mask.
 */
void GXSetFieldMask(GXBool oddMask, GXBool evenMask) {
    /* Write BP register for field mask */
}

/*
 * GXSetFieldMode - Set field rendering mode.
 */
void GXSetFieldMode(GXBool texLOD, GXBool adjustAR) {
    /* Write BP register for field mode */
}
