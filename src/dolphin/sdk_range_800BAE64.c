/**
 * @file sdk_range_800BAE64.c
 * @brief dolphin-sdk code, 0x800BAE64 - 0x800BB2E4 (7 fns).
 *
 * Standalone GXTexture range reconstructed from the matching Dolphin SDK
 * implementation shared with Pokemon XD.
 */
#include "dolphin/types.h"

typedef struct GXTexObj_800BAFFC {
    u32 mode0;
    u32 mode1;
    u32 image0;
    u32 image3;
    void* userData;
    u32 format;
    u32 tlutName;
    u16 loadCount;
    u8 loadFormat;
    u8 flags;
} GXTexObj_800BAFFC;

typedef struct GXTexRegion_800BAFFC {
    u32 image1;
    u32 image2;
    u16 sizeEven;
    u16 sizeOdd;
    u8 is32bMipmap;
    u8 isCached;
} GXTexRegion_800BAFFC;

typedef struct GXTlutObj_800BB050 {
    u32 format;
    u32 loadTlut;
    u16 numEntries;
} GXTlutObj_800BB050;

typedef struct GXTlutRegion_800BB098 {
    u32 tmemAddrConf;
    GXTlutObj_800BB050 tlutObj;
} GXTlutRegion_800BB098;

typedef enum GXTexCacheSize_800BB134 {
    GX_TEXCACHE_32K_800BB134,
    GX_TEXCACHE_128K_800BB134,
    GX_TEXCACHE_512K_800BB134,
    GX_TEXCACHE_NONE_800BB134,
} GXTexCacheSize_800BB134;

typedef struct GXData_800BAFFC {
    u16 vertexCountNot;
    u16 bpSentNot;
    u8 pad_004[0x40C];
    GXTexRegion_800BAFFC* (*texRegionCallback)(GXTexObj_800BAFFC*, u32);
    GXTlutRegion_800BB098* (*tlutRegionCallback)(u32);
    u8 pad_418[0x44];
    u32 textureImage0[8];
    u32 textureMode0[8];
    u8 pad_49C[0x58];
    u32 dirtyState;
} GXData_800BAFFC;

extern GXData_800BAFFC* const gx;
extern u8 lbl_80478A78[8];
extern u8 lbl_80478A80[8];
extern u8 lbl_80478A88[8];
extern u8 lbl_80478A90[8];
extern u8 lbl_80478A98[8];
extern u8 lbl_80478AA0[8];
extern u8 lbl_80478AA8[8];

#define GX_FIFO_U8  (*(volatile u8*)0xCC008000)
#define GX_FIFO_U32 (*(volatile u32*)0xCC008000)

#define GX_WRITE_BP(value)     \
    do {                       \
        GX_FIFO_U8 = 0x61;     \
        GX_FIFO_U32 = (value); \
    } while (0)

void GXLoadTexObjPreLoaded(GXTexObj_800BAFFC* obj, GXTexRegion_800BAFFC* region,
                 u32 mapID) {
    GXTlutRegion_800BB098* tlutRegion;

    obj->mode0 = (obj->mode0 & 0x00FFFFFF) | (lbl_80478A78[mapID] << 24);
    obj->mode1 = (obj->mode1 & 0x00FFFFFF) | (lbl_80478A80[mapID] << 24);
    obj->image0 = (obj->image0 & 0x00FFFFFF) | (lbl_80478A88[mapID] << 24);
    region->image1 =
        (region->image1 & 0x00FFFFFF) | (lbl_80478A90[mapID] << 24);
    region->image2 =
        (region->image2 & 0x00FFFFFF) | (lbl_80478A98[mapID] << 24);
    obj->image3 = (obj->image3 & 0x00FFFFFF) | (lbl_80478AA0[mapID] << 24);

    GX_WRITE_BP(obj->mode0);
    GX_WRITE_BP(obj->mode1);
    GX_WRITE_BP(obj->image0);
    GX_WRITE_BP(region->image1);
    GX_WRITE_BP(region->image2);
    GX_WRITE_BP(obj->image3);

    if ((obj->flags & 2) == 0) {
        tlutRegion = gx->tlutRegionCallback(obj->tlutName);
        tlutRegion->tlutObj.format =
            (tlutRegion->tlutObj.format & 0x00FFFFFF) |
            (lbl_80478AA8[mapID] << 24);
        GX_WRITE_BP(tlutRegion->tlutObj.format);
    }

    gx->textureImage0[mapID] = obj->image0;
    gx->textureMode0[mapID] = obj->mode0;
    gx->dirtyState |= 1;
    gx->bpSentNot = 0;
}

void GXLoadTexObj(GXTexObj_800BAFFC* obj, u32 mapID) {
    GXTexRegion_800BAFFC* region = gx->texRegionCallback(obj, mapID);

    GXLoadTexObjPreLoaded(obj, region, mapID);
}

void GXInitTlutObj(GXTlutObj_800BB050* obj, void* data, u32 format,
                 u16 numEntries) {
    obj->format = 0;
    obj->format = (obj->format & ~0xC00) | (format << 10);
    obj->loadTlut =
        (obj->loadTlut & 0xFFE00000) | (((u32)data & 0x3FFFFFFF) >> 5);
    obj->loadTlut = (obj->loadTlut & 0x00FFFFFF) | 0x64000000;
    obj->numEntries = numEntries;
}

void GXLoadTlut(GXTlutObj_800BB050* obj, u32 tlutName) {
    extern void __GXFlushTextureState(void);
    GXTlutRegion_800BB098* region = gx->tlutRegionCallback(tlutName);
    u32 tmemOffset;

    __GXFlushTextureState();
    *(volatile u8*)0xCC008000 = 0x61;
    *(volatile u32*)0xCC008000 = obj->loadTlut;
    *(volatile u8*)0xCC008000 = 0x61;
    *(volatile u32*)0xCC008000 = region->tmemAddrConf;
    __GXFlushTextureState();

    tmemOffset = region->tmemAddrConf;
    obj->format = (obj->format & ~0x3FF) | (tmemOffset & 0x3FF);
    region->tlutObj = *obj;
}

void GXInitTexCacheRegion(GXTexRegion_800BAFFC* region, u8 is32bMipmap,
                          u32 tmemEven, GXTexCacheSize_800BB134 sizeEven,
                          u32 tmemOdd, GXTexCacheSize_800BB134 sizeOdd) {
    u32 widthExponent;

    switch (sizeEven) {
    case GX_TEXCACHE_32K_800BB134:
        widthExponent = 3;
        break;
    case GX_TEXCACHE_128K_800BB134:
        widthExponent = 4;
        break;
    case GX_TEXCACHE_512K_800BB134:
        widthExponent = 5;
        break;
    default:
        break;
    }

    region->image1 = 0;
    region->image1 = (region->image1 & 0xFFFF8000) | (tmemEven >> 5);
    region->image1 =
        (region->image1 & 0xFFFC7FFF) | (widthExponent << 15);
    region->image1 =
        (region->image1 & 0xFFE3FFFF) | (widthExponent << 18);
    region->image1 &= 0xFFDFFFFF;

    switch (sizeOdd) {
    case GX_TEXCACHE_32K_800BB134:
        widthExponent = 3;
        break;
    case GX_TEXCACHE_128K_800BB134:
        widthExponent = 4;
        break;
    case GX_TEXCACHE_512K_800BB134:
        widthExponent = 5;
        break;
    case GX_TEXCACHE_NONE_800BB134:
        widthExponent = 0;
        break;
    default:
        break;
    }

    region->image2 = 0;
    region->image2 = (region->image2 & 0xFFFF8000) | (tmemOdd >> 5);
    region->image2 =
        (region->image2 & 0xFFFC7FFF) | (widthExponent << 15);
    region->image2 =
        (region->image2 & 0xFFE3FFFF) | (widthExponent << 18);
    region->is32bMipmap = is32bMipmap;
    region->isCached = 1;
}

void GXInitTlutRegion(GXTlutRegion_800BB098* region, u32 tmemAddr,
                      u32 tlutSize) {
    region->tmemAddrConf = 0;
    region->tmemAddrConf =
        (region->tmemAddrConf & ~0x3FF) | ((tmemAddr - 0x80000) >> 9);
    region->tmemAddrConf =
        (region->tmemAddrConf & 0xFFE003FF) | (tlutSize << 10);
    region->tmemAddrConf =
        (region->tmemAddrConf & 0x00FFFFFF) | 0x65000000;
}

void GXInvalidateTexAll(void) {
    extern void __GXFlushTextureState(void);

    __GXFlushTextureState();
    *(volatile u8*)0xCC008000 = 0x61;
    *(volatile u32*)0xCC008000 = 0x66001000;
    *(volatile u8*)0xCC008000 = 0x61;
    *(volatile u32*)0xCC008000 = 0x66001100;
    __GXFlushTextureState();
}
