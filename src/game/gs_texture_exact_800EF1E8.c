/**
 * @file gs_texture_exact_800EF1E8.c
 * @brief GStextureConvertFromHW (0x800EF1E8 - 0x800EF3E0): copies the EFB
 *        into a render-target texture (clipped to the current render mode's
 *        framebuffer), with the texture locked for the copy.
 *
 * Function-boundary carve of the GStexture TU (see gs_texture.c), on the
 * unit's flags (GC/1.3 -O4,p). Text only; the display size comes from the
 * HSD video state through HAL's HSD_VIGetRenderMode inline.
 *
 * The texture-copy SDK calls (GXSetTexCopySrc source rectangle, GXSetTexCopyDst
 * destination size/format/mipmap, GXCopyTex copy) are declared with the
 * prototypes of their linked unit (src/dolphin/sdk_range_800B9578.c). The
 * mipmap flag is passed as GX_TRUE/GX_FALSE; a bare comparison gives the
 * same instructions but a different register allocation.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSCache.h"
#include "game/gs_texture.h"
#include "sysdolphin/baselib/video.h"

/* Dolphin SDK GXEnum.h */
#ifndef GX_TRUE
#define GX_FALSE ((GXBool)0)
#define GX_TRUE ((GXBool)1)
#endif

void GXInvalidateTexAll(void);
extern void GXSetTexCopySrc(u16 left, u16 top, u16 width, u16 height);
extern void GXSetTexCopyDst(u16 width, u16 height, s32 format, u8 mipmap);
extern void GXCopyTex(void* dest, u8 clear);
extern void GXPixModeSync(void);

/* 0x800EF1E8 | 0x1F8 */
u8 GStextureConvertFromHW(GStextureHandle* tex, u8 clear) {
    void* image;
    s32 gxFormat;
    s32 width;
    s32 height;
    GXRenderModeObj* rmode;

    switch (tex->format) {
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x90:
    case 0xA0:
        break;
    default:
        return 0;
    }

    rmode = HSD_VIGetRenderMode();
    tex->refCount++;
    image = tex->mipData[0];
    switch (tex->format) {
    case 0: gxFormat = 8; break;
    case 1: gxFormat = 9; break;
    case 0x30: gxFormat = 0xA; break;
    case 0x40: gxFormat = 0; break;
    case 0x41: gxFormat = 2; break;
    case 0x42: gxFormat = 1; break;
    case 0x43: gxFormat = 3; break;
    case 0x44: gxFormat = 4; break;
    case 0x45: gxFormat = 6; break;
    case 0x90: gxFormat = 5; break;
    case 0xB0: gxFormat = 0xE; break;
    case 0xA0: gxFormat = 0x27; break;
    default: gxFormat = -1; break;
    }

    width = rmode->fbWidth;
    height = rmode->efbHeight;
    /* RULE-EXCEPTION(title-path): no-op (s32) casts, only for retail's load order - see docs/RULE_EXCEPTIONS.md */
    if ((s32)tex->width < width) width = tex->width;
    if ((s32)tex->height < height) height = tex->height;

    GXSetTexCopySrc(0, 0, width, height);
    GXSetTexCopyDst(width, height, gxFormat, tex->mipLevels > 1 ? GX_TRUE : GX_FALSE);
    GXSetZMode(1, 3, 1);
    GXCopyTex(image, clear);
    GXPixModeSync();
    GXInvalidateTexAll();
    DCFlushRange(tex->mipData[0], tex->totalSize);
    GXInvalidateTexAll();
    tex->refCount--;
    return 1;
}
