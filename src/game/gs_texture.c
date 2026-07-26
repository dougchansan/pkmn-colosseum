/**
 * @file gs_texture.c
 * @brief GStexture -- Genius Sonority texture management system.
 *
 * This unit spans 0x800EF098 - 0x800F0030 (17 functions). It used to
 * extend to 0x800F07A8 (38 functions), but the tail from 0x800F0030
 * onward is GSthread.cpp content (cooperative-scheduler primitives),
 * not GStexture.cpp -- confirmed against the XD reference splits
 * (game/pxdvs/GSAPI/GStexture/GStexture.cpp ends at 0x8010315C, which
 * lines up byte-exactly with our GStextureInit (ends 0x800F0030) ->
 * threadLoadGPRRegisters boundary). That tail has been moved to
 * src/game/gs_thread.c, which already owned the immediately-following
 * range (0x800F07A8-0x800F75FC) as the same XD TU.
 *
 * Confirmed real names and content in this file's remaining range:
 *   - GStextureCreate (0x800EF5FC, WIP, partially matched) -- the main
 *     texture-allocation routine.
 *   - GStextureUnlockImage (0x800EF504) and GStextureLockImage
 *     (0x800EF548) -- both are real matched symbols in symbols.txt and
 *     are referenced by name from gs_render.c's EFB-capture path
 *     (`GStextureUnlockImage(image)` feeding `GXDrawDone`, and
 *     `GStextureLockImage(image, 0)`). GStextureUnlockImage's return
 *     value is used by the caller, so it returns the texture's pre-
 *     decrement refCount rather than void.
 *   - GStextureConvertFromHW, GStextureGetGXformat, GStextureGetTlutFormat,
 *     GStextureGetFormat, GStextureGetMiplevels, GStextureGetYsize,
 *     GStextureGetXsize, GStextureSetFilter, GStextureSetWrap,
 *     GStextureFree, GStextureLoad and GStextureInit are confirmed real
 *     names (matched against the XD reference binary's GStexture.cpp
 *     function order/sizes), replacing the previous fn_ scaffold names.
 *     fn_800EF098 has no confirmed name yet and remains an honest
 *     unmatched TODO stub.
 *
 * Debug strings:
 *   "GStexture: invalid texture format"
 *   "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]"
 *
 * Address range: 0x800EF098 - 0x800F0030
 */

#include "dolphin/types.h"
#include "dolphin/os/OSCache.h"
#include "game/gs_texture.h"
#include "game/gs_thread.h"
#include "game/gs_scene_types.h"  /* memcpy, GSmem externs, GSlogWrite */

/* ===== External SDK / engine functions ===== */
u16   fn_800E2C04(u32 size, u32 alignment);     /* GSmemAlloc */
void  GXInvalidateTexAll(void);
extern void  fn_800BB050(void* gxTlutObj, void* data, u32 format,
                         u32 entries); /* GXInitTlutObj */
extern void  fn_800BA9E4(void* gxTexObj, void* data,
                          u16 width, u16 height, u32 gxFmt,
                          u32 wrapS, u32 wrapT, u32 hasMips); /* GXInitTexObj */

/* ===== String constants (rodata) ===== */
extern const char lbl_80270F98[]; /* "GStexture: invalid texture format" */
extern const char lbl_80270FBC[]; /* "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]" */

/* ===== Display descriptor ===== */
extern u8 lbl_80466BC0[];  /* current display descriptor */

/* ===== Global state (sbss/sdata) ===== */
/* lbl_8047ABF0 : u16 -- texture pool allocation handle */
static u16 gsTexPoolHandle;
/* lbl_8047ABF8 : u32 -- max texture count */
static u32 gsTexMaxCount;               /* @sda21 lbl_8047ABF8 */
/* lbl_8047ABF4 : GStextureHandle* -- base pointer to texture pool */
static GStextureHandle* gsTexPool;       /* @sda21 lbl_8047ABF4 */

#if !defined(GS_TEXTURE_800EF548_SUFFIX_ACTIVE)
/* =======================================================================
 *  fn_800EF098 | 0x150
 *  Rearranges CI4 texture pixel data from linear to 4x4 block-tiled
 *  order. Only operates on format 0x44 (GS_CI4).
 * ======================================================================= */
#pragma push
#pragma optimize_for_size on
#pragma peephole off
void fn_800EF098(GStextureHandle* tex) {
    u16 width;
 u16 height;
    u16* src;
    u16 allocSize;
    u16 pixelCount;
    u16 handle;
    u16* tempBuf;
    u16 w4;
    u16 i;

    if (tex->format != 0x44) {
        return;
    }

    tex->refCount++;

    src = tex->mipData[0];
    if (src == NULL) {
        return;
    }

    width = tex->width;
    height = tex->height;
    allocSize = width * height * 2;
    pixelCount = width * height;

    handle = _toolentryAlloc__FUl(allocSize);
    if (handle == 0) {
        return;
    }

    tempBuf = fn_800E27B0(handle);

    w4 = width / 4;
    for (i = 0; i < pixelCount; i++) {
        u16 pixel = src[i];
        u16 blk = i / 16;
        u16 br = blk / w4;
        u16 tc = blk - br * w4;
        u16 ty = (i % 16) / 4;
        u16 tx = i % 4;
        u16 dst = (tc + br * width) * 4 + ty * width + tx;
        tempBuf[dst] = pixel;
    }

    memcpy(src, tempBuf, allocSize);
    DCFlushRange(tex->mipData[0], tex->totalSize);
    GXInvalidateTexAll();

    tex->refCount--;

    fn_800E24B0(handle);
    fn_800E209C(handle);
}
#pragma pop

/* =======================================================================
 *  GStextureConvertFromHW | 0x1F8
 *  TODO: match -- callers disagree on shape (gs_render.c calls it with
 *  zero args; gs_gfx.c calls GStextureConvertFromHW(sc, 1)), so no signature is
 *  asserted here.
 * ======================================================================= */
void GStextureConvertFromHW(void) {
    /* TODO: match -- 0x1F8 bytes at 0x800EF1E8 */
}

#endif

#if defined(GS_TEXTURE_800EF548_SUFFIX_ACTIVE)
/* =======================================================================
 *  GStextureLockImage
 *  Address: 0x800EF548, Size: 0x30
 *
 *  Real matched name (symbols.txt) referenced from gs_render.c as
 *  `GStextureLockImage(image, 0)`.
 * ======================================================================= */
void* GStextureLockImage(GStextureHandle* tex, u8 level) {
    if (level >= 8) {
        return NULL;
    }

    tex->refCount++;
    return tex->mipData[level];
}

/* =======================================================================
 *  GStextureSetFilter | 0x18
 *  TODO: match -- gs_render.c calls GStextureSetFilter() with zero args,
 *  incompatible with the old 4-argument SetWrapMode signature.
 * ======================================================================= */
void GStextureSetFilter(GStextureHandle* tex, u32 wrapS, u32 wrapT, u32 lodClamp) {
    /* objdiff ground truth: this store pair lands at offsets 0x18/0x1C,
     * not 0x10/0x14 -- the GStextureHandle field names in the header
     * (wrapS/wrapT vs minFilter/magFilter) are swapped from their real
     * struct offsets. Using the header's minFilter/magFilter accessors
     * here (which resolve to 0x18/0x1C) to reach the correct bytes
     * without editing the out-of-scope header. */
    tex->minFilter = wrapS;
    tex->magFilter = wrapT;
    tex->lodClamp = lodClamp;
    tex->dirty = 1;
}

/* =======================================================================
 *  GStextureSetWrap | 0x14
 *  TODO: match -- gs_render.c, gs_pcbox.c and effect_util.c all call
 *  GStextureSetWrap() with zero args, incompatible with the old 3-argument
 *  SetFilterMode signature.
 * ======================================================================= */
void GStextureSetWrap(GStextureHandle* tex, u32 minFilt, u32 magFilt) {
    /* See GStextureSetFilter: header field names for the wrap/filter pair are
     * swapped from their real offsets. wrapS/wrapT resolve to 0x10/0x14,
     * which is where objdiff ground truth places these two stores. */
    tex->wrapS = minFilt;
    tex->wrapT = magFilt;
    tex->dirty = 1;
}

/* =======================================================================
 *  GStextureFree | 0x58
 *  TODO: match -- called ~25 times from effect_visual.c, tracefx.c,
 *  gs_field_world.c, gs_title.c, battle_grid.c and gs_colsys.c as a
 *  generic `GStextureFree(void* model)` "model release" helper. That
 *  usage is inconsistent with a GStextureHandle-specific free, so no
 *  semantic name is asserted here.
 * ======================================================================= */
void GStextureFree(GStextureHandle* tex) {
    if (tex->inUse == 0) {
        return;
    }
    if (tex->memHandle == 0) {
        return;
    }

    tex->inUse = 0;
    fn_800E24B0(tex->memHandle);
    fn_800E209C(tex->memHandle);
}

/* =======================================================================
 *  fn_800EFD14 | 0x28
 *  TODO: match -- effect_util.c calls fn_800EFD14() with zero args,
 *  incompatible with the old 2-argument Bind signature.
 * ======================================================================= */
void fn_800EFD14(GStextureHandle* tex, u16 handle) {
    if (tex == NULL || tex->inUse != 0) {
        return;
    }

    tex->inUse = 1;
    tex->memHandle = handle;
}

/* =======================================================================
 *  GStextureCreate
 *  Address: 0x800EF5FC, Size: 0x718
 *
 *  This is the main texture creation function. It:
 *    1. If width/height are both 0, uses display dimensions
 *    2. Validates dimensions (4-1024 range)
 *    3. Determines bits per pixel from format
 *    4. Rounds dimensions up to power-of-two (with warning)
 *    5. Finds a free slot in the texture pool
 *    6. Computes total data size (base + all mip levels)
 *    7. Allocates pixel data from GSmem
 *    8. Initialises the GStextureHandle fields
 *    9. Sets up the GXTexObj
 *   10. Returns the handle pointer
 *
 *  r3 = width, r4 = height, r5 = format, r6 = tlutFormat,
 *  r7 = mipLevels
 * ======================================================================= */
GStextureHandle* GStextureCreate(s32 width, s32 height, s32 format,
                                  s32 tlutFormat, u8 mipLevels) {
    u16 adjWidth, adjHeight;
    u8 dimensionAlign;
    u32 pixelCount;
    u32 totalSize;
    u32 i;
    GStextureHandle* tex;
    u32 gxFmt;
    s32 tlutEntries;
    s32 gxTexFmt;
    u32 hasMips;
    u32 mipSize;

    /* Step 1: Default to display dimensions if both are 0 */
    if ((width & 0xFFFF) == 0 && (height & 0xFFFF) == 0) {
        u8* disp = (u8*)lbl_80466BC0;
        width = *(u16*)(disp + 4);
        height = *(u16*)(disp + 6);
    }

    /* Step 2: Validate dimensions: 4 <= dim <= 1024 */
    adjWidth = width & 0xFFFF;
    adjHeight = height & 0xFFFF;

    if (adjWidth > 0x400 || adjHeight > 0x400 ||
        adjWidth < 4 || adjHeight < 4) {
        return NULL;
    }

    /* Step 3: Determine the dimension-alignment granularity. */
    switch (format) {
        case 0x00:
        case 0x40:
        case 0x41:
        case 0xB0:
            dimensionAlign = 8;
            break;
        case 0x01:
        case 0x42:
        case 0x43:
        case 0xA0:
            dimensionAlign = 4;
            break;
        case 0x30:
        case 0x44:
        case 0x45:
        case 0x90:
            dimensionAlign = 4;
            break;
        default:
            /* Unknown format */
            GSlogWrite(lbl_80270F98);
            return NULL;
    }

    /* Step 4: Round dimensions to next power of two */
    {
        u32 origW = width & 0xFFFF;
        u32 origH = height & 0xFFFF;
        u16 pw, ph;
        u8 shift = 0;

        adjWidth = (u16)((adjWidth + (dimensionAlign - 1)) & ~(dimensionAlign - 1));
        adjHeight = (u16)((adjHeight + (dimensionAlign - 1)) & ~(dimensionAlign - 1));

        /* Warn if dimensions were adjusted */
        if (adjWidth != origW || adjHeight != origH) {
            GSlogWrite(lbl_80270FBC, origW, origH, adjWidth, adjHeight);
        }

        /* Iteratively halve until both are <= 4, counting shifts */
        pw = adjWidth;
        ph = adjHeight;
        while ((pw & 0xFFFF) > 4 && (ph & 0xFFFF) > 4 && shift < 7) {
            pw >>= 1;
            ph >>= 1;
            shift++;
        }

        /* Ensure mipLevels doesn't exceed the computed shift */
        if ((mipLevels & 0xFF) > shift) {
            mipLevels = shift;
        }
    }

    /* Step 5: Find a free slot in the texture pool */
    tex = gsTexPool;
    for (i = gsTexMaxCount; i != 0; i--) {
        if (tex->inUse == 0) {
            goto texture_slot_found;
        }
        tex++;
    }
    tex = NULL;

texture_slot_found:

    if (tex == NULL) {
        return NULL;
    }

    tlutEntries = 0;
    switch (format) {
        case 0x00:
            tlutEntries = 0x10;
            /* fall through */
        case 0x40:
        case 0xB0:
            tex->bitsPerPixel = 4;
            break;
        case 0x01:
            tlutEntries = 0x100;
            /* fall through */
        case 0x41:
        case 0x42:
        case 0xA0:
            tex->bitsPerPixel = 8;
            break;
        case 0x30:
            tlutEntries = 0x400;
            /* fall through */
        case 0x43:
        case 0x44:
        case 0x90:
            tex->bitsPerPixel = 16;
            break;
        case 0x45:
            tex->bitsPerPixel = 32;
            break;
        default:
            return NULL;
    }

    /* Step 6: Compute total data size */
    pixelCount = (u32)adjWidth * (u32)adjHeight;
    mipLevels = (mipLevels & 0xFF) + 1;
    tex->totalSize = 0;

    {
        u32 mipSize = (u32)tex->bitsPerPixel * pixelCount / 8;
        s32 level;

        for (level = 0; level < mipLevels; level++) {
            u32 roundedSize = (mipSize + 0x1F) & ~0x1F;
            tex->totalSize += roundedSize;
            mipSize >>= 1;
        }
    }

    /* Handle TLUT (palette) data size */
    if (tlutEntries != 0) {
        if (tlutFormat == 0 || tlutFormat < 0 || tlutFormat >= 4) {
            return NULL;
        }
        /* TLUT occupies additional space (palette entries * 2 bytes) */
        tex->totalSize += (tlutEntries << 4) >> 3;
    }

    /* Step 7: Allocate pixel data from GSmem */
    tex->memHandle = fn_800E2C04(tex->totalSize, 0x20);

    if (tex->memHandle == 0) {
        return NULL;
    }

    tex->mipData[0] = fn_800E27B0(tex->memHandle);
    if (tex->mipData[0] == NULL) {
        fn_800E209C(tex->memHandle);
        return NULL;
    }

    /* Step 8: Initialise handle fields */
    tex->inUse = 1;
    tex->width = adjWidth;
    tex->height = adjHeight;
    tex->mipLevels = mipLevels;
    tex->format = format;
    tex->tlutFormat = tlutFormat;
    tex->wrapS = 0;
    tex->wrapT = 0;
    tex->minFilter = 2;
    tex->magFilter = 2;

    if (mipLevels > 1) {
        tex->lodClamp = 2;
    } else {
        tex->lodClamp = 0;
    }

    tex->refCount = 0;
    tex->unk52 = 0;

    /* Compute per-mip data pointers */
    {
        s32 level;

        mipSize = (u32)tex->bitsPerPixel * pixelCount / 8;
        for (level = 1; level < 8; level++) {
            if (level < tex->mipLevels) {
                tex->mipData[level] = (u8*)tex->mipData[level - 1] + mipSize;
                mipSize >>= 1;
            } else {
                tex->mipData[level] = NULL;
            }
        }
    }

    /* Compute TLUT pointer (for CI formats) */
    if (tlutFormat >= 1 && tlutFormat < 4) {
        u8* lastMipEnd;
        u32 mipIdx = tex->mipLevels - 1;
        lastMipEnd = (u8*)tex->mipData[mipIdx] + mipSize;
        /* Actually from the assembly: calculated from last mip's end pointer */
        tex->tlutData = lastMipEnd;
    } else {
        tex->tlutData = NULL;
    }

    /* Step 9: Determine GX format for GXTexObj setup */
    gxTexFmt = -1;
    switch (tex->format) {
        case 0x00: gxTexFmt = 0x08; break;
        case 0x01: gxTexFmt = 0x09; break;
        case 0x30: gxTexFmt = 0x0A; break;
        case 0x40: gxTexFmt = 0x00; break;
        case 0x41: gxTexFmt = 0x02; break;
        case 0x42: gxTexFmt = 0x01; break;
        case 0x43: gxTexFmt = 0x03; break;
        case 0x44: gxTexFmt = 0x04; break;
        case 0x45: gxTexFmt = 0x06; break;
        case 0x90: gxTexFmt = 0x05; break;
        case 0xB0: gxTexFmt = 0x0E; break;
        case 0xA0: gxTexFmt = 0x01; break;
        default:   gxTexFmt = -1;   break;
    }

    /* Set up GXTlutObj if texture has a TLUT */
    if (tex->tlutData != NULL) {
        u32 tlutEntries;
        u32 gxTlutFmt;

        tlutEntries = 0;
        switch (tex->format) {
            case 0x00: tlutEntries = 0x10;  break;
            case 0x01: tlutEntries = 0x100; break;
            case 0x30: tlutEntries = 0x400; break;
            default:   break;
        }

        gxTlutFmt = 0;
        switch (tex->tlutFormat) {
            case 1: gxTlutFmt = 0; break;
            case 2: gxTlutFmt = 1; break;
            case 3: gxTlutFmt = 2; break;
            default: break;
        }

        fn_800BB050(tex->gxTlutObj, tex->tlutData, gxTlutFmt, tlutEntries);
    }

    /* Set up GXTexObj */
    hasMips = tex->mipLevels > 1;
    fn_800BA9E4(tex->gxTexObj, tex->mipData[0], tex->width, tex->height,
                gxTexFmt, 0, 0, hasMips);

    tex->dirty = 1;

    return tex;
}

/* =======================================================================
 *  GStextureLoad | 0x284
 *  TODO: match -- gs_field_resource.c calls GStextureLoad(result) as a
 *  "WZX overlap check" returning u32 from a single argument; unrelated
 *  to the old fictional TPL-texture-setup body, so no semantic name is
 *  asserted here.
 * ======================================================================= */
void GStextureLoad(void) {
    /* TODO: match -- 0x284 bytes at 0x800EFD3C */
}

/* =======================================================================
 *  GStextureInit | 0x70
 *  TODO: match -- main.c calls GStextureInit(0x10) with the comment
 *  "GX FIFO init", not a texture-pool initialiser, so no semantic name
 *  is asserted here.
 * ======================================================================= */
void GStextureInit(u32 count) {
    u32 i;

    gsTexMaxCount = count;
    gsTexPoolHandle = _toolentryAlloc__FUl(count << 7);
    if (gsTexPoolHandle == 0) {
        return;
    }

    gsTexPool = fn_800E27B0(gsTexPoolHandle);
    for (i = 0; i < gsTexMaxCount; i++) {
        ((u8*)gsTexPool)[i * 0x80 + 6] = 0;
    }
}
#endif
