/**
 * @file sdk_range_800B9578.c
 * @brief Dolphin SDK GX state and frame-buffer routines, 0x800B9578-0x800BA198.
 */

#include "dolphin/types.h"

typedef struct GXData_800B9578 {
    /* 0x000 */ u16 field_000;
    /* 0x002 */ u16 bpSentNot;
    /* 0x004 */ u8 pad_004[0x1CC];
    /* 0x1D0 */ u32 cmode0;
    /* 0x1D4 */ u32 cmode1;
    /* 0x1D8 */ u32 zmode;
    /* 0x1DC */ u32 peCtrl;
    /* 0x1E0 */ u32 cpDispSrc;
    /* 0x1E4 */ u32 cpDispSize;
    /* 0x1E8 */ u32 cpDispStride;
    /* 0x1EC */ u32 cpDisp;
    /* 0x1F0 */ u32 cpTexSrc;
    /* 0x1F4 */ u32 cpTexSize;
    /* 0x1F8 */ u32 cpTexStride;
    /* 0x1FC */ u32 cpTex;
    /* 0x200 */ u8 cpTexZ;
    /* 0x201 */ u8 pad_201[3];
    /* 0x204 */ u32 genMode;
} GXData_800B9578;

typedef struct GXColor_800B9BDC {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor_800B9BDC;

extern GXData_800B9578* const gx;
extern u32 __cvt_fp2unsigned(f32 value);
extern void __GetImageTileCount(s32 format, u16 width, u16 height,
                                u32* rowTiles, u32* columnTiles, u32* planes);

typedef union PPCWGPipe_800B9578 {
    u8 u8;
    u16 u16;
    u32 u32;
    s8 s8;
    s16 s16;
    s32 s32;
    f32 f32;
    f64 f64;
} PPCWGPipe_800B9578;

volatile PPCWGPipe_800B9578 GXWGFifo_800B9578 : 0xCC008000;

#define GX_FIFO_U8  GXWGFifo_800B9578.u8
#define GX_FIFO_U32 GXWGFifo_800B9578.u32

#define GX_BP_REG(reg)       \
    do {                     \
        GX_FIFO_U8 = 0x61;   \
        GX_FIFO_U32 = (reg); \
    } while (0)

#define GX_GET_REG_FIELD(reg, size, shift) \
    ((s32)((reg) >> (shift)) & ((1 << (size)) - 1))
#define GX_SET_REG_FIELD(reg, size, shift, value)                         \
    ((reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) |          \
             ((u32)(value) << (shift)))

void __GXSetGenMode(void)
{
    GX_BP_REG(gx->genMode);
    gx->bpSentNot = 0;
}

void GXSetDispCopySrc(u16 left, u16 top, u16 width, u16 height)
{
    gx->cpDispSrc = 0;
    GX_SET_REG_FIELD(gx->cpDispSrc, 10, 0, left);
    GX_SET_REG_FIELD(gx->cpDispSrc, 10, 10, top);
    GX_SET_REG_FIELD(gx->cpDispSrc, 8, 24, 0x49);

    gx->cpDispSize = 0;
    GX_SET_REG_FIELD(gx->cpDispSize, 10, 0, width - 1);
    GX_SET_REG_FIELD(gx->cpDispSize, 10, 10, height - 1);
    GX_SET_REG_FIELD(gx->cpDispSize, 8, 24, 0x4A);
}

void GXSetTexCopySrc(u16 left, u16 top, u16 width, u16 height)
{
    gx->cpTexSrc = 0;
    GX_SET_REG_FIELD(gx->cpTexSrc, 10, 0, left);
    GX_SET_REG_FIELD(gx->cpTexSrc, 10, 10, top);
    GX_SET_REG_FIELD(gx->cpTexSrc, 8, 24, 0x49);

    gx->cpTexSize = 0;
    GX_SET_REG_FIELD(gx->cpTexSize, 10, 0, width - 1);
    GX_SET_REG_FIELD(gx->cpTexSize, 10, 10, height - 1);
    GX_SET_REG_FIELD(gx->cpTexSize, 8, 24, 0x4A);
}

void GXSetDispCopyDst(u16 width, u16 height)
{
    u16 stride;

    stride = (int)width * 2;
    gx->cpDispStride = 0;
    GX_SET_REG_FIELD(gx->cpDispStride, 10, 0, stride >> 5);
    GX_SET_REG_FIELD(gx->cpDispStride, 8, 24, 0x4D);
}

void GXSetTexCopyDst(u16 width, u16 height, s32 format, u8 mipmap)
{
    u32 rowTiles;
    u32 columnTiles;
    u32 planes;
    u32 peFormat;
    u32 peFormatHigh;

    gx->cpTexZ = 0;
    peFormat = format & 0xF;

    if (format == 0x13) {
        peFormat = 0xB;
    }

    switch (format) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 0x26:
        GX_SET_REG_FIELD(gx->cpTex, 2, 15, 3);
        break;
    default:
        GX_SET_REG_FIELD(gx->cpTex, 2, 15, 2);
        break;
    }

    gx->cpTexZ = (0x10 == (format & 0x10));
    peFormatHigh = (peFormat >> 3) & 1;
    !peFormat;
    GX_SET_REG_FIELD(gx->cpTex, 1, 3, peFormatHigh);
    peFormat &= 7;

    __GetImageTileCount(format, width, height, &rowTiles, &columnTiles,
                        &planes);

    gx->cpTexStride = 0;
    GX_SET_REG_FIELD(gx->cpTexStride, 10, 0, rowTiles * planes);
    GX_SET_REG_FIELD(gx->cpTexStride, 8, 24, 0x4D);
    GX_SET_REG_FIELD(gx->cpTex, 1, 9, mipmap);
    GX_SET_REG_FIELD(gx->cpTex, 3, 4, peFormat);
}

void GXSetDispCopyFrame2Field(u32 value)
{
    GX_SET_REG_FIELD(gx->cpDisp, 2, 12, value);
    GX_SET_REG_FIELD(gx->cpTex, 2, 12, 0);
}

void GXSetCopyClamp(u32 value)
{
    u8 clampBottom;
    u8 clampTop;

    clampTop = (value & 1) == 1;
    clampBottom = (value & 2) == 2;

    GX_SET_REG_FIELD(gx->cpDisp, 1, 0, clampTop);
    GX_SET_REG_FIELD(gx->cpDisp, 1, 1, clampBottom);
    GX_SET_REG_FIELD(gx->cpTex, 1, 0, clampTop);
    GX_SET_REG_FIELD(gx->cpTex, 1, 1, clampBottom);
}

static inline u32 __GXGetNumXfbLines(u32 efbHeight, u32 scale)
{
    u32 count;
    u32 realHeight;
    u32 scaleDivisor;

    count = (efbHeight - 1) * 0x100;
    realHeight = (count / scale) + 1;
    scaleDivisor = scale;

    if (scaleDivisor > 0x80 && scaleDivisor < 0x100) {
        while ((scaleDivisor & 1) == 0) {
            scaleDivisor >>= 1;
        }
        if ((efbHeight % scaleDivisor) == 0) {
            realHeight++;
        }
    }

    if (realHeight > 0x400) {
        realHeight = 0x400;
    }

    return realHeight;
}

f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight)
{
    f32 lastScale;
    f32 scale;
    u32 integerScale;
    u32 targetHeight;
    u32 realHeight;

    targetHeight = xfbHeight;
    scale = (f32)xfbHeight / (f32)efbHeight;
    integerScale = (u32)(256.0f / scale) & 0x1FF;
    realHeight = __GXGetNumXfbLines(efbHeight, integerScale);

    while (realHeight > xfbHeight) {
        targetHeight--;
        scale = (f32)targetHeight / (f32)efbHeight;
        integerScale = (u32)(256.0f / scale) & 0x1FF;
        realHeight = __GXGetNumXfbLines(efbHeight, integerScale);
    }

    lastScale = scale;
    while (realHeight < xfbHeight) {
        lastScale = scale;
        targetHeight++;
        scale = (f32)targetHeight / (f32)efbHeight;
        integerScale = (u32)(256.0f / scale) & 0x1FF;
        realHeight = __GXGetNumXfbLines(efbHeight, integerScale);
    }

    return lastScale;
}

u32 GXSetDispCopyYScale(f32 scale)
{
    u8 enable;
    u32 integerScale;
    u32 height;
    u32 reg;

    integerScale = (u32)(256.0f / scale) & 0x1FF;
    enable = integerScale != 0x100;

    reg = 0;
    GX_SET_REG_FIELD(reg, 9, 0, integerScale);
    GX_SET_REG_FIELD(reg, 8, 24, 0x4E);
    GX_BP_REG(reg);
    gx->bpSentNot = 0;
    GX_SET_REG_FIELD(gx->cpDisp, 1, 10, enable);
    height = (u32)GX_GET_REG_FIELD(gx->cpDispSize, 10, 10) + 1;
    return __GXGetNumXfbLines(height, integerScale);
}

void GXSetCopyClear(GXColor_800B9BDC color, u32 clearZ)
{
    u32 reg;

    reg = 0;
    GX_SET_REG_FIELD(reg, 8, 0, color.r);
    GX_SET_REG_FIELD(reg, 8, 8, color.a);
    GX_SET_REG_FIELD(reg, 8, 24, 0x4F);
    GX_BP_REG(reg);

    reg = 0;
    GX_SET_REG_FIELD(reg, 8, 0, color.b);
    GX_SET_REG_FIELD(reg, 8, 8, color.g);
    GX_SET_REG_FIELD(reg, 8, 24, 0x50);
    GX_BP_REG(reg);

    reg = 0;
    GX_SET_REG_FIELD(reg, 24, 0, clearZ);
    GX_SET_REG_FIELD(reg, 8, 24, 0x51);
    GX_BP_REG(reg);
    gx->bpSentNot = 0;
}

void GXSetCopyFilter(u8 antialias, const u8 samplePattern[12][2], u8 verticalFilter,
                 const u8 filter[7])
{
    u32 sampleLocations[4];
    u32 coefficient0;
    u32 coefficient1;

    if (antialias != 0) {
        sampleLocations[0] = 0;
        GX_SET_REG_FIELD(sampleLocations[0], 4, 0, samplePattern[0][0]);
        GX_SET_REG_FIELD(sampleLocations[0], 4, 4, samplePattern[0][1]);
        GX_SET_REG_FIELD(sampleLocations[0], 4, 8, samplePattern[1][0]);
        GX_SET_REG_FIELD(sampleLocations[0], 4, 12, samplePattern[1][1]);
        GX_SET_REG_FIELD(sampleLocations[0], 4, 16, samplePattern[2][0]);
        GX_SET_REG_FIELD(sampleLocations[0], 4, 20, samplePattern[2][1]);
        GX_SET_REG_FIELD(sampleLocations[0], 8, 24, 1);

        sampleLocations[1] = 0;
        GX_SET_REG_FIELD(sampleLocations[1], 4, 0, samplePattern[3][0]);
        GX_SET_REG_FIELD(sampleLocations[1], 4, 4, samplePattern[3][1]);
        GX_SET_REG_FIELD(sampleLocations[1], 4, 8, samplePattern[4][0]);
        GX_SET_REG_FIELD(sampleLocations[1], 4, 12, samplePattern[4][1]);
        GX_SET_REG_FIELD(sampleLocations[1], 4, 16, samplePattern[5][0]);
        GX_SET_REG_FIELD(sampleLocations[1], 4, 20, samplePattern[5][1]);
        GX_SET_REG_FIELD(sampleLocations[1], 8, 24, 2);

        sampleLocations[2] = 0;
        GX_SET_REG_FIELD(sampleLocations[2], 4, 0, samplePattern[6][0]);
        GX_SET_REG_FIELD(sampleLocations[2], 4, 4, samplePattern[6][1]);
        GX_SET_REG_FIELD(sampleLocations[2], 4, 8, samplePattern[7][0]);
        GX_SET_REG_FIELD(sampleLocations[2], 4, 12, samplePattern[7][1]);
        GX_SET_REG_FIELD(sampleLocations[2], 4, 16, samplePattern[8][0]);
        GX_SET_REG_FIELD(sampleLocations[2], 4, 20, samplePattern[8][1]);
        GX_SET_REG_FIELD(sampleLocations[2], 8, 24, 3);

        sampleLocations[3] = 0;
        GX_SET_REG_FIELD(sampleLocations[3], 4, 0, samplePattern[9][0]);
        GX_SET_REG_FIELD(sampleLocations[3], 4, 4, samplePattern[9][1]);
        GX_SET_REG_FIELD(sampleLocations[3], 4, 8, samplePattern[10][0]);
        GX_SET_REG_FIELD(sampleLocations[3], 4, 12, samplePattern[10][1]);
        GX_SET_REG_FIELD(sampleLocations[3], 4, 16, samplePattern[11][0]);
        GX_SET_REG_FIELD(sampleLocations[3], 4, 20, samplePattern[11][1]);
        GX_SET_REG_FIELD(sampleLocations[3], 8, 24, 4);
    } else {
        sampleLocations[0] = 0x01666666;
        sampleLocations[1] = 0x02666666;
        sampleLocations[2] = 0x03666666;
        sampleLocations[3] = 0x04666666;
    }

    GX_BP_REG(sampleLocations[0]);
    GX_BP_REG(sampleLocations[1]);
    GX_BP_REG(sampleLocations[2]);
    GX_BP_REG(sampleLocations[3]);

    coefficient0 = 0;
    GX_SET_REG_FIELD(coefficient0, 8, 24, 0x53);
    coefficient1 = 0;
    GX_SET_REG_FIELD(coefficient1, 8, 24, 0x54);

    if (verticalFilter != 0) {
        GX_SET_REG_FIELD(coefficient0, 6, 0, filter[0]);
        GX_SET_REG_FIELD(coefficient0, 6, 6, filter[1]);
        GX_SET_REG_FIELD(coefficient0, 6, 12, filter[2]);
        GX_SET_REG_FIELD(coefficient0, 6, 18, filter[3]);
        GX_SET_REG_FIELD(coefficient1, 6, 0, filter[4]);
        GX_SET_REG_FIELD(coefficient1, 6, 6, filter[5]);
        GX_SET_REG_FIELD(coefficient1, 6, 12, filter[6]);
    } else {
        GX_SET_REG_FIELD(coefficient0, 6, 0, 0);
        GX_SET_REG_FIELD(coefficient0, 6, 6, 0);
        GX_SET_REG_FIELD(coefficient0, 6, 12, 21);
        GX_SET_REG_FIELD(coefficient0, 6, 18, 22);
        GX_SET_REG_FIELD(coefficient1, 6, 0, 21);
        GX_SET_REG_FIELD(coefficient1, 6, 6, 0);
        GX_SET_REG_FIELD(coefficient1, 6, 12, 0);
    }

    GX_BP_REG(coefficient0);
    GX_BP_REG(coefficient1);
    gx->bpSentNot = 0;
}

void GXSetDispCopyGamma(u32 value)
{
    GX_SET_REG_FIELD(gx->cpDisp, 2, 7, value);
}

void GXCopyDisp(void* destination, u8 clear)
{
    u32 reg;
    u32 temporaryPeControl;
    u32 physicalAddress;
    u8 changePeControl;

    if (clear) {
        reg = gx->zmode;
        GX_SET_REG_FIELD(reg, 1, 0, 1);
        GX_SET_REG_FIELD(reg, 3, 1, 7);
        GX_BP_REG(reg);

        reg = gx->cmode0;
        GX_SET_REG_FIELD(reg, 1, 0, 0);
        GX_SET_REG_FIELD(reg, 1, 1, 0);
        GX_BP_REG(reg);
    }

    changePeControl = 0;
    if ((clear || (u32)GX_GET_REG_FIELD(gx->peCtrl, 3, 0) == 3) &&
        (u32)GX_GET_REG_FIELD(gx->peCtrl, 1, 6) == 1) {
        changePeControl = 1;
        temporaryPeControl = gx->peCtrl;
        GX_SET_REG_FIELD(temporaryPeControl, 1, 6, 0);
        GX_BP_REG(temporaryPeControl);
    }

    GX_BP_REG(gx->cpDispSrc);
    GX_BP_REG(gx->cpDispSize);
    GX_BP_REG(gx->cpDispStride);

    physicalAddress = (u32)destination & 0x3FFFFFFF;
    reg = 0;
    GX_SET_REG_FIELD(reg, 21, 0, physicalAddress >> 5);
    GX_SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_BP_REG(reg);

    GX_SET_REG_FIELD(gx->cpDisp, 1, 11, clear);
    GX_SET_REG_FIELD(gx->cpDisp, 1, 14, 1);
    GX_SET_REG_FIELD(gx->cpDisp, 8, 24, 0x52);
    GX_BP_REG(gx->cpDisp);

    if (clear) {
        GX_BP_REG(gx->zmode);
        GX_BP_REG(gx->cmode0);
    }
    if (changePeControl) {
        GX_BP_REG(gx->peCtrl);
    }
    gx->bpSentNot = 0;
}

void GXCopyTex(void* destination, u8 clear)
{
    u32 reg;
    u32 temporaryPeControl;
    u32 physicalAddress;
    u8 changePeControl;

    if (clear) {
        reg = gx->zmode;
        GX_SET_REG_FIELD(reg, 1, 0, 1);
        GX_SET_REG_FIELD(reg, 3, 1, 7);
        GX_BP_REG(reg);

        reg = gx->cmode0;
        GX_SET_REG_FIELD(reg, 1, 0, 0);
        GX_SET_REG_FIELD(reg, 1, 1, 0);
        GX_BP_REG(reg);
    }

    changePeControl = 0;
    temporaryPeControl = gx->peCtrl;
    if (gx->cpTexZ && ((temporaryPeControl & 7) != 3)) {
        changePeControl = 1;
        GX_SET_REG_FIELD(temporaryPeControl, 3, 0, 3);
    }
    if ((clear || ((temporaryPeControl & 7) == 3)) &&
        (((temporaryPeControl >> 6) & 1) == 1)) {
        changePeControl = 1;
        GX_SET_REG_FIELD(temporaryPeControl, 1, 6, 0);
    }
    if (changePeControl) {
        GX_BP_REG(temporaryPeControl);
    }

    GX_BP_REG(gx->cpTexSrc);
    GX_BP_REG(gx->cpTexSize);
    GX_BP_REG(gx->cpTexStride);

    physicalAddress = (u32)destination & 0x3FFFFFFF;
    reg = 0;
    GX_SET_REG_FIELD(reg, 21, 0, physicalAddress >> 5);
    GX_SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_BP_REG(reg);

    GX_SET_REG_FIELD(gx->cpTex, 1, 11, clear);
    GX_SET_REG_FIELD(gx->cpTex, 1, 14, 0);
    GX_SET_REG_FIELD(gx->cpTex, 8, 24, 0x52);
    GX_BP_REG(gx->cpTex);

    if (clear) {
        GX_BP_REG(gx->zmode);
        GX_BP_REG(gx->cmode0);
    }
    if (changePeControl) {
        GX_BP_REG(gx->peCtrl);
    }
    gx->bpSentNot = 0;
}

void GXClearBoundingBox(void)
{
    u32 reg;

    reg = 0x550003FF;
    GX_BP_REG(reg);
    reg = 0x560003FF;
    GX_BP_REG(reg);
    gx->bpSentNot = 0;
}
