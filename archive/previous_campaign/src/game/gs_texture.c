/**
 * @file gs_texture.c
 * @brief GStexture -- Genius Sonority texture management system.
 *
 * Decompiled from:
 *   fn_800EFFC0 (GStextureInit)
 *   GStextureCreate (GStextureCreate)
 *   fn_800EF098 (GStextureConvertCI)
 *   fn_800EF1E8 (GStextureUploadFromBuffer)
 *   fn_800EF3E0 (GStextureGetGXFormat)
 *   fn_800EF4D4 (GStextureGetTLUTFormat)
 *   fn_800EF4DC (GStextureGetFormat)
 *   fn_800EF4E4 (GStextureGetMipCount)
 *   fn_800EF4F4 (GStextureGetHeight)
 *   fn_800EF4FC (GStextureGetWidth)
 *   fn_800EF504 (GStextureFlush)
 *   fn_800EF548 (GStextureGetMipData)
 *   fn_800EF578 (GStextureSetWrapMode)
 *   fn_800EF590 (GStextureSetFilterMode)
 *   fn_800EF5A4 (GStextureFree)
 *   fn_800EFD14 (GStextureBind)
 *   fn_800EFD3C (GStextureSetupFromTPL)
 *
 * Debug strings:
 *   "GStexture: invalid texture format"
 *   "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]"
 *
 * Address range: 0x800EF098 - 0x800F07A8 (approx.)
 */

#include "dolphin/types.h"
#include "game/gs_texture.h"
#include "game/gs_thread.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);        /* OSReport */
extern u16   GSmemAllocRaw(u32 size);                  /* fn_800E3534 */
extern void* GSmemGetPtr(u16 handle);                  /* fn_800E27B0 */
extern void* GSmemLock(u16 handle);                    /* fn_800E24B0 */
extern void  GSmemFree(u16 handle);                    /* fn_800E209C */
extern u16   GSmemAlloc(u32 alignment, u32 size);      /* fn_800E2C04 */
extern void  DCFlushRange(void* addr, u32 len);
extern void  memcpy(void* dst, const void* src, u32 len);
extern void  fn_800BB29C(void);                        /* GXInvalidateTexAll */
extern void  fn_800BB050(void* gxTlutObj, void* data, u32 format); /* GXInitTlutObj */
extern void  fn_800BA9E4(void* gxTexObj, void* data,
                          u16 width, u16 height, u32 gxFmt,
                          u32 wrapS, u32 wrapT, u32 hasMips); /* GXInitTexObj */
extern void  fn_800B962C(u32 a, u32 b, u16 width, u16 height); /* GXSetScissor or viewport */
extern void  fn_800B96F8(u16 width, u16 height, u32 fmt,
                          u32 mipFlag);                /* GXSetCopyTexSrc */
extern void  fn_800BCE88(u32 a, u32 b, u32 c);        /* GX copy / capture */
extern void  fn_800B9FE4(void* data, void* srcBuf);   /* GXCopyTex */
extern void  fn_800B8E74(void);                        /* GXPixModeSync */

/* ===== String constants (rodata) ===== */
extern const char lbl_80270F98[]; /* "GStexture: invalid texture format" */
extern const char lbl_80270FBC[]; /* "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]" */

/* ===== Display descriptor ===== */
extern u8 lbl_80466BC0[];  /* current display descriptor */

/* ===== Global state (sbss/sdata) ===== */
/* lbl_8047ABF8 : u32 -- max texture count */
static u32 gsTexMaxCount;               /* @sda21 lbl_8047ABF8 */
/* lbl_8047ABF4 : GStextureHandle* -- base pointer to texture pool */
static GStextureHandle* gsTexPool;       /* @sda21 lbl_8047ABF4 */
/* lbl_8047ABF0 : u16 -- GSmem handle for texture pool */
static u16 gsTexPoolHandle;              /* @sda21 lbl_8047ABF0 */

/* =======================================================================
 *  GStextureInit / fn_800EFFC0
 *  Address: 0x800EFFC0, Size: 0x70
 *
 *  Allocates the texture pool from GSmem and zeroes all entries.
 *
 *  Assembly:
 *    stw r3, lbl_8047ABF8@sda21(r0)   ; store maxTextures
 *    slwi r3, r3, 7                   ; r3 = maxTextures * 0x80
 *    bl GSmemAllocRaw                 ; allocate pool
 *    sth r3, lbl_8047ABF0@sda21(r0)   ; store handle
 *    beq fail                         ; if handle == 0, bail
 *    bl GSmemGetPtr                   ; resolve to pointer
 *    stw r3, lbl_8047ABF4@sda21(r0)   ; store pool pointer
 *    ; loop: clear all entries' inUse byte (offset 0x06)
 *    for (i = 0; i < maxTextures; i++) {
 *        pool[i * 0x80 + 6] = 0;
 *    }
 * ======================================================================= */
void GStextureInit(u32 maxTextures) {
    u16 handle;
    u32 i;
    u32 offset;

    gsTexMaxCount = maxTextures;

    /* Allocate pool: maxTextures * 0x80 bytes */
    handle = GSmemAllocRaw(maxTextures << 7);
    gsTexPoolHandle = handle;

    if ((handle & 0xFFFF) == 0) {
        return;
    }

    gsTexPool = (GStextureHandle*)GSmemGetPtr(handle);

    /* Zero out all slots' inUse flag */
    offset = 0;
    for (i = 0; i < gsTexMaxCount; i++) {
        u8* base = (u8*)gsTexPool;
        base[offset + 6] = 0;  /* inUse = 0 */
        offset += 0x80;
    }
}

/* =======================================================================
 *  GStextureGetWidth / fn_800EF4FC
 *  Address: 0x800EF4FC, Size: 0x8
 * ======================================================================= */
u16 GStextureGetWidth(GStextureHandle* tex) {
    return tex->width;
}

/* =======================================================================
 *  GStextureGetHeight / fn_800EF4F4
 *  Address: 0x800EF4F4, Size: 0x8
 * ======================================================================= */
u16 GStextureGetHeight(GStextureHandle* tex) {
    return tex->height;
}

/* =======================================================================
 *  GStextureGetFormat / fn_800EF4DC
 *  Address: 0x800EF4DC, Size: 0x8
 * ======================================================================= */
u32 GStextureGetFormat(GStextureHandle* tex) {
    return tex->format;
}

/* =======================================================================
 *  GStextureGetTLUTFormat / fn_800EF4D4
 *  Address: 0x800EF4D4, Size: 0x8
 * ======================================================================= */
u32 GStextureGetTLUTFormat(GStextureHandle* tex) {
    return tex->tlutFormat;
}

/* =======================================================================
 *  GStextureGetMipCount / fn_800EF4E4
 *  Address: 0x800EF4E4, Size: 0x10
 *
 *  Assembly:
 *    lbz r3, 0x5(r3)     ; load mipLevels
 *    subi r0, r3, 0x1    ; subtract 1
 *    clrlwi r3, r0, 24   ; mask to byte
 *    blr
 * ======================================================================= */
u8 GStextureGetMipCount(GStextureHandle* tex) {
    return (u8)(tex->mipLevels - 1);
}

/* =======================================================================
 *  GStextureSetWrapMode / fn_800EF578
 *  Address: 0x800EF578, Size: 0x18
 *
 *  Assembly:
 *    stw r4, 0x18(r3)    ; wrapS
 *    li r0, 1
 *    stw r5, 0x1C(r3)    ; wrapT
 *    stw r6, 0x20(r3)    ; lodClamp
 *    stb r0, 0x07(r3)    ; dirty = 1
 *    blr
 * ======================================================================= */
void GStextureSetWrapMode(GStextureHandle* tex, u32 wrapS, u32 wrapT,
                           u32 lodClamp) {
    tex->wrapS = wrapS;
    tex->wrapT = wrapT;
    tex->lodClamp = lodClamp;
    tex->dirty = 1;
}

/* =======================================================================
 *  GStextureSetFilterMode / fn_800EF590
 *  Address: 0x800EF590, Size: 0x14
 *
 *  Assembly:
 *    stw r4, 0x10(r3)    ; minFilter
 *    li r0, 1
 *    stw r5, 0x14(r3)    ; magFilter
 *    stb r0, 0x07(r3)    ; dirty = 1
 *    blr
 * ======================================================================= */
void GStextureSetFilterMode(GStextureHandle* tex, u32 minFilt, u32 magFilt) {
    tex->minFilter = minFilt;
    tex->magFilter = magFilt;
    tex->dirty = 1;
}

/* =======================================================================
 *  GStextureFree / fn_800EF5A4
 *  Address: 0x800EF5A4, Size: 0x58
 *
 *  Assembly:
 *    lbz r0, 0x6(r3)       ; load inUse
 *    beq done               ; if 0, already free
 *    lhz r0, 0x24(r31)     ; load memHandle
 *    beq done               ; if 0, no allocation
 *    li r0, 0
 *    stb r0, 0x6(r31)      ; inUse = 0
 *    lhz r3, 0x24(r31)     ; load handle
 *    bl GSmemLock           ; lock it (for validation)
 *    lhz r3, 0x24(r31)     ; load handle again
 *    bl GSmemFree           ; free it
 * ======================================================================= */
void GStextureFree(GStextureHandle* tex) {
    if (tex->inUse == 0) {
        return;
    }
    if (tex->memHandle == 0) {
        return;
    }

    tex->inUse = 0;
    GSmemLock(tex->memHandle);
    GSmemFree(tex->memHandle);
}

/* =======================================================================
 *  GStextureFlush / fn_800EF504
 *  Address: 0x800EF504, Size: 0x44
 *
 *  Assembly:
 *    lwz r3, 0x28(r31)     ; data pointer
 *    lwz r4, 0x4C(r31)     ; totalSize
 *    bl DCFlushRange
 *    bl GXInvalidateTexAll
 *    lhz r3, 0x50(r31)     ; refCount
 *    subi r0, r3, 1
 *    sth r0, 0x50(r31)     ; refCount--
 * ======================================================================= */
void GStextureFlush(GStextureHandle* tex) {
    DCFlushRange(tex->data, tex->totalSize);
    fn_800BB29C();  /* GXInvalidateTexAll */

    tex->refCount--;
}

/* =======================================================================
 *  GStextureGetMipData / fn_800EF548
 *  Address: 0x800EF548, Size: 0x30
 *
 *  Assembly:
 *    clrlwi r0, r4, 24     ; level &= 0xFF
 *    cmplwi r0, 8           ; if level >= 8
 *    blt ok
 *    li r3, 0               ; return NULL
 *    blr
 *  ok:
 *    lhz r5, 0x50(r3)      ; refCount
 *    slwi r0, r4, 2         ; level * 4
 *    add r4, r3, r0
 *    addi r0, r5, 1
 *    sth r0, 0x50(r3)      ; refCount++
 *    lwz r3, 0x28(r4)      ; data + mipOffsets[level]
 *    blr
 * ======================================================================= */
void* GStextureGetMipData(GStextureHandle* tex, u8 level) {
    if (level >= 8) {
        return NULL;
    }

    tex->refCount++;
    return tex->mipOffsets[level] + (u8*)tex;  /* offsetted read at 0x28 + level*4 */
}

/* =======================================================================
 *  GStextureBind / fn_800EFD14
 *  Address: 0x800EFD14, Size: 0x28
 *
 *  Assembly:
 *    cmplwi r3, 0           ; NULL check
 *    beqlr
 *    lbz r0, 0x6(r3)       ; check inUse
 *    beq set                ; if not in use, set it
 *    blr                    ; already in use, bail
 *  set:
 *    li r0, 1
 *    stb r0, 0x6(r3)       ; inUse = 1
 *    sth r4, 0x24(r3)      ; memHandle = handle
 *    blr
 * ======================================================================= */
void GStextureBind(GStextureHandle* tex, u16 memHandle) {
    if (tex == NULL) {
        return;
    }
    if (tex->inUse != 0) {
        return;  /* already bound */
    }

    tex->inUse = 1;
    tex->memHandle = memHandle;
}

/* =======================================================================
 *  GStextureGetGXFormat / fn_800EF3E0
 *  Address: 0x800EF3E0, Size: 0xF4
 *
 *  Maps the GS internal format to the GXTexFmt hardware enum.
 *  Large switch statement on tex->format (offset 0x08).
 *
 *  Assembly (simplified switch):
 *    lwz r0, 0x8(r3)
 *    switch(r0):
 *      case 0x00: return 0x08  (GX_TF_I4)
 *      case 0x01: return 0x09  (GX_TF_I8)
 *      case 0x30: return 0x0A  (GX_TF_IA4)
 *      case 0x40: return 0x00  (GX_TF_I4... special)
 *      case 0x41: return 0x02  (GX_TF_RGB565)
 *      case 0x42: return 0x01  (GX_TF_RGB5A3)
 *      case 0x43: return 0x03  (GX_TF_RGBA8)
 *      case 0x44: return 0x04  (GX_TF_C4)
 *      case 0x45: return 0x06  (GX_TF_C14X2)
 *      case 0x90: return 0x05  (GX_TF_C8)
 *      case 0xA0: if alpha==0 return 0x27, else return 0x01
 *      case 0xB0: return 0x0E  (GX_TF_CMPR)
 *      default:   return -1
 * ======================================================================= */
s32 GStextureGetGXFormat(GStextureHandle* tex, u8 alpha) {
    u32 fmt = tex->format;

    switch (fmt) {
        case 0x00: return 0x08;   /* GX_TF_I4 */
        case 0x01: return 0x09;   /* GX_TF_I8 */
        case 0x30: return 0x0A;   /* GX_TF_IA4 */
        case 0x40: return 0x00;   /* GX_TF_I4 variant */
        case 0x41: return 0x02;   /* GX_TF_RGB565 */
        case 0x42: return 0x01;   /* GX_TF_RGB5A3 */
        case 0x43: return 0x03;   /* GX_TF_RGBA8 */
        case 0x44: return 0x04;   /* GX_TF_C4 */
        case 0x45: return 0x06;   /* GX_TF_C14X2 */
        case 0x90: return 0x05;   /* GX_TF_C8 */
        case 0xB0: return 0x0E;   /* GX_TF_CMPR */
        case 0xA0:
            if (alpha == 0) {
                return 0x27;      /* special alpha-only format */
            }
            return 0x01;          /* GX_TF_RGB5A3 */
        default:
            return -1;
    }
}

/* =======================================================================
 *  GStextureCreate / GStextureCreate
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
GStextureHandle* GStextureCreate(u16 width, u16 height, u32 format,
                                  u32 tlutFormat, u8 mipLevels) {
    u16 adjWidth, adjHeight;
    u8 bpp;
    u32 pixelCount;
    u32 totalSize;
    u8 mipCount;
    u32 i;
    GStextureHandle* tex;
    u16 handle;
    u32 gxFmt;
    u32 tlutPalSize;
    s32 gxTexFmt;
    u32 hasMips;

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

    /* Step 3: Determine bits per pixel from format */
    switch (format) {
        case 0x00:  /* I4: 4bpp */
        case 0x40:  /* IA4: 4bpp */
        case 0xB0:  /* CMPR: 4bpp */
            bpp = 8;
            break;
        case 0x01:  /* I8: 8bpp */
        case 0x41:  /* RGB565 */
        case 0x42:  /* RGB5A3 */
        case 0xA0:  /* A8 */
            bpp = 4;
            break;
        case 0x30:  /* IA8: 16bpp */
        case 0x90:  /* CI8: palette */
            bpp = 4;
            break;
        case 0x43:  /* RGBA8: 32bpp */
            bpp = 16;
            break;
        case 0x44:  /* CI4 */
            bpp = 8;
            break;
        case 0x45:  /* CI14x2 */
            bpp = 32;
            break;
        default:
            /* Unknown format */
            fn_800DD970(lbl_80270F98);
            return NULL;
    }

    /* Step 4: Round dimensions to next power of two */
    {
        u32 origW = width & 0xFFFF;
        u32 origH = height & 0xFFFF;
        u32 pw = adjWidth, ph = adjHeight;
        u8 shift = 0;

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

        adjWidth = (u16)((adjWidth + (bpp - 1)) & ~(bpp - 1));
        adjHeight = (u16)((adjHeight + (bpp - 1)) & ~(bpp - 1));

        /* Warn if dimensions were adjusted */
        if (adjWidth != origW || adjHeight != origH) {
            fn_800DD970(lbl_80270FBC, origW, origH, adjWidth, adjHeight);
        }
    }

    /* Step 5: Find a free slot in the texture pool */
    tex = gsTexPool;
    {
        u32 count = gsTexMaxCount;
        u32 idx;
        GStextureHandle* slot = gsTexPool;
        for (idx = 0; idx < count; idx++) {
            if (slot->inUse == 0) {
                tex = slot;
                break;
            }
            slot = (GStextureHandle*)((u8*)slot + 0x80);
        }
        if (idx >= count) {
            tex = NULL;
        }
    }

    if (tex == NULL) {
        return NULL;
    }

    /* Step 6: Compute total data size */
    pixelCount = (u32)adjWidth * (u32)adjHeight;
    mipCount = (mipLevels & 0xFF) + 1;
    tex->totalSize = 0;

    {
        u32 mipSize = (u32)bpp * pixelCount / 8;
        u32 level;

        for (level = 0; level < mipCount; level++) {
            u32 roundedSize = (mipSize + 0x1F) & ~0x1F;
            tex->totalSize += roundedSize;
            mipSize >>= 2;  /* each mip is 1/4 the size */
        }
    }

    /* Handle TLUT (palette) data size */
    tlutPalSize = 0;
    if (pixelCount != 0 && tlutFormat > 0 && tlutFormat < 4) {
        /* TLUT occupies additional space (palette entries * 2 bytes) */
        tlutPalSize = pixelCount * 16 / 8;  /* from assembly: slwi r0, r0, 4; srawi r0, r0, 3 */
        tex->totalSize += tlutPalSize;
    } else if (tlutFormat == 0 || tlutFormat >= 4) {
        return NULL;
    }

    /* Step 7: Allocate pixel data from GSmem */
    handle = GSmemAlloc(0x20, tex->totalSize);
    tex->memHandle = handle;

    if ((handle & 0xFFFF) == 0) {
        return NULL;
    }

    tex->data = GSmemGetPtr(handle);
    if (tex->data == NULL) {
        GSmemFree(tex->memHandle);
        return NULL;
    }

    /* Step 8: Initialise handle fields */
    tex->inUse = 1;
    tex->width = adjWidth;
    tex->height = adjHeight;
    tex->mipLevels = mipLevels;
    tex->format = format;
    tex->tlutFormat = tlutFormat;
    tex->minFilter = 0;
    tex->magFilter = 0;
    tex->wrapS = 2;  /* GX_CLAMP */
    tex->wrapT = 2;  /* GX_CLAMP */

    if (mipCount > 1) {
        tex->lodClamp = 2;
    } else {
        tex->lodClamp = 0;
    }

    tex->refCount = 0;
    tex->pad52 = 0;

    /* Compute per-mip data offsets */
    {
        u32 mipSize = (u32)bpp * pixelCount / 8;
        u32 cumOffset = 0;
        u32 level;

        for (level = 0; level < 7; level++) {
            if ((u32)(level + 1) < (u32)tex->mipLevels) {
                tex->mipOffsets[level + 1] = tex->mipOffsets[level] + mipSize;
                mipSize >>= 2;
            } else {
                tex->mipOffsets[level + 1] = 0;
            }
        }
    }

    /* Compute TLUT offset (for CI formats) */
    if (tlutFormat >= 1 && tlutFormat < 4) {
        u32 lastMipEnd;
        u32 mipIdx = tex->mipLevels;
        lastMipEnd = tex->mipOffsets[mipIdx] + (u32)bpp * pixelCount / 8;
        /* Actually from the assembly: calculated from last mip's end offset */
        tex->tlutOffset = lastMipEnd;
    } else {
        tex->tlutOffset = 0;
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
    if (tex->tlutOffset != 0) {
        u32 palFmt;
        u32 tlutWrap;

        palFmt = 0;
        switch (tex->format) {
            case 0x00: palFmt = 0x10;  break;
            case 0x01: palFmt = 0x100; break;
            case 0x30: palFmt = 0x400; break;
            default:   break;
        }

        tlutWrap = 0;
        switch (tex->tlutFormat) {
            case 1: tlutWrap = 0; break;
            case 2: tlutWrap = 1; break;
            case 3: tlutWrap = 2; break;
            default: break;
        }

        fn_800BB050(tex->gxTlutObj, (void*)((u32)tex->data + tex->tlutOffset),
                     tlutWrap);
    }

    /* Set up GXTexObj */
    hasMips = (tex->mipLevels == 1) ? 0 : 1;
    fn_800BA9E4(tex->gxTexObj, tex->data, tex->width, tex->height,
                gxTexFmt, 0, 0, hasMips);

    tex->dirty = 1;

    return tex;
}

/* =======================================================================
 *  GStextureConvertCI / fn_800EF098
 *  Address: 0x800EF098, Size: 0x150
 *
 *  Converts a CI (colour-indexed, format 0x44) texture to direct colour.
 *  Allocates a temporary buffer, unswizzles the CI data, copies back,
 *  flushes dcache, and frees the temporary buffer.
 *
 *  Only operates on format 0x44 (CI4) textures.
 * ======================================================================= */
void GStextureConvertCI(GStextureHandle* tex) {
    u32 fmt;
    u16 pixelCount;
    u32 bufSize;
    u16 tmpHandle;
    u8* tmpData;
    u16 w;
    u32 i;

    fmt = tex->format;
    if (fmt != 0x44) {
        return;
    }

    /* Increment ref count to prevent eviction during conversion */
    tex->refCount++;

    tmpData = (u8*)tex->data;
    if (tmpData == NULL) {
        return;
    }

    w = tex->width;
    pixelCount = (u16)(w * tex->height);
    bufSize = (u32)pixelCount << 1;

    /* Allocate temporary buffer */
    tmpHandle = GSmemAllocRaw(bufSize);
    if ((tmpHandle & 0xFFFF) == 0) {
        return;
    }

    {
        u8* dst = (u8*)GSmemGetPtr(tmpHandle);
        u16* srcPalette = (u16*)tmpData;
        u16 blockW = w >> 2;

        /* Unswizzle CI4 data: iterate over all pixels, look up palette entries,
         * and write direct-colour pixels to the temp buffer.
         *
         * The CI4 format stores 2 pixels per byte in 4x4 blocks.
         * This loop reconstructs the unswizzled linear pixel order. */
        for (i = 0; i < pixelCount; i++) {
            u16 blockX, blockY, subX, subY;
            u16 palEntry;
            u32 srcIdx, dstIdx;

            blockY = (u16)((i >> 4) / blockW);
            blockX = (u16)((i >> 4) % blockW);
            subY = (u16)((i >> 2) & 3);
            subX = (u16)(i & 3);

            /* Read palette index from source */
            palEntry = srcPalette[i];

            /* Compute destination position in linear layout */
            dstIdx = (blockY * w + subY) + (blockX * 4 + subX);
            dstIdx <<= 1;

            /* Write to temp buffer */
            dst[dstIdx]     = (u8)(palEntry >> 8);
            dst[dstIdx + 1] = (u8)(palEntry & 0xFF);
        }

        /* Copy converted data back to original buffer */
        memcpy(tmpData, dst, bufSize);
    }

    /* Flush and invalidate */
    DCFlushRange(tex->data, tex->totalSize);
    fn_800BB29C();

    /* Clean up temporary allocation */
    tex->refCount--;
    GSmemLock(tmpHandle);
    GSmemFree(tmpHandle);
}

/* =======================================================================
 *  GStextureUploadFromBuffer / fn_800EF1E8
 *  Address: 0x800EF1E8, Size: 0x1F8
 *
 *  Copies pixel data from srcBuffer into the texture via GX EFB capture.
 *  Only works for paletted/special formats: 0x40, 0x41-0x43, 0x90, 0xA0.
 *
 *  Steps:
 *    1. Validate format is supported
 *    2. Increment refCount
 *    3. Map internal format to GXTexFmt
 *    4. Read display descriptor for viewport clamp
 *    5. Set up GX scissor/copy state
 *    6. Copy from srcBuffer into the texture's pixel data
 *    7. Flush dcache and invalidate texture cache
 *    8. Decrement refCount
 * ======================================================================= */
u32 GStextureUploadFromBuffer(GStextureHandle* tex, void* srcBuffer) {
    u32 fmt;
    s32 gxFmt;
    u16 copyW, copyH;
    u8* disp;

    fmt = tex->format;

    /* Only certain formats support EFB upload */
    if (fmt != 0x90 && fmt != 0x40 && fmt != 0x41 && fmt != 0x42 &&
        fmt != 0x43 && fmt != 0xA0) {
        return 0;
    }

    tex->refCount++;

    disp = (u8*)lbl_80466BC0;

    /* Map format to GXTexFmt */
    switch (fmt) {
        case 0x00: gxFmt = 0x08; break;
        case 0x01: gxFmt = 0x09; break;
        case 0x30: gxFmt = 0x0A; break;
        case 0x40: gxFmt = 0x00; break;
        case 0x41: gxFmt = 0x02; break;
        case 0x42: gxFmt = 0x01; break;
        case 0x43: gxFmt = 0x03; break;
        case 0x44: gxFmt = 0x04; break;
        case 0x45: gxFmt = 0x06; break;
        case 0x90: gxFmt = 0x05; break;
        case 0xB0: gxFmt = 0x0E; break;
        case 0xA0: gxFmt = 0x27; break;
        default:   gxFmt = -1;   break;
    }

    /* Clamp copy dimensions to display size */
    copyW = *(u16*)(disp + 4);
    copyH = *(u16*)(disp + 6);
    if (tex->width < copyW) {
        copyW = tex->width;
    }
    if (tex->height < copyH) {
        copyH = tex->height;
    }

    /* Set GX state for EFB copy */
    fn_800B962C(0, 0, copyW, copyH);

    {
        u32 mipFlag = (tex->mipLevels == 1) ? 0 : 1;
        fn_800B96F8(copyW, copyH, gxFmt, mipFlag);
    }

    fn_800BCE88(1, 3, 1);

    /* Copy data from src to texture */
    fn_800B9FE4(tex->data, srcBuffer);

    fn_800B8E74();     /* GXPixModeSync */
    fn_800BB29C();     /* GXInvalidateTexAll */

    /* Flush and invalidate */
    DCFlushRange(tex->data, tex->totalSize);
    fn_800BB29C();

    tex->refCount--;

    return 1;
}

/* =======================================================================
 *  GStextureSetupFromTPL / fn_800EFD3C
 *  Address: 0x800EFD3C, Size: 0x284
 *
 *  Sets up a texture from TPL (Texture Palette Library) data.
 *  Resolves relative pointers in the TPL data block to absolute
 *  pointers by adding the texture handle base address.
 *
 *  Steps:
 *    1. For each mip level (up to mipLevels), add the base address
 *       to the mip data offset to produce an absolute pointer.
 *    2. Zero out remaining mip offset slots.
 *    3. If TLUT offset is non-zero, add base to make absolute.
 *    4. Map format to GXTexFmt.
 *    5. Set up GXTlutObj if applicable.
 *    6. Set up GXTexObj with the resolved data pointer.
 *    7. Mark dirty = 1, clear memHandle to 0.
 * ======================================================================= */
GStextureHandle* GStextureSetupFromTPL(GStextureHandle* tex) {
    u32 i;
    u32 level;
    s32 gxFmt;
    u32 hasMips;

    /* Step 1: Resolve relative mip data offsets to absolute pointers */
    for (level = 0; (s32)level < (s32)tex->mipLevels; level++) {
        tex->mipOffsets[level] = (u32)tex + tex->mipOffsets[level];
    }

    /* Step 2: Zero out remaining mip slots (up to 8 total) */
    for (i = tex->mipLevels; i < 8; i++) {
        tex->mipOffsets[i] = 0;
    }

    /* Step 3: Resolve TLUT offset to absolute pointer */
    if (tex->tlutOffset != 0) {
        tex->tlutOffset = (u32)tex + tex->tlutOffset;
    }

    /* Step 4: Map format to GXTexFmt */
    gxFmt = -1;
    switch (tex->format) {
        case 0x00: gxFmt = 0x08; break;
        case 0x01: gxFmt = 0x09; break;
        case 0x30: gxFmt = 0x0A; break;
        case 0x40: gxFmt = 0x00; break;
        case 0x41: gxFmt = 0x02; break;
        case 0x42: gxFmt = 0x01; break;
        case 0x43: gxFmt = 0x03; break;
        case 0x44: gxFmt = 0x04; break;
        case 0x45: gxFmt = 0x06; break;
        case 0x90: gxFmt = 0x05; break;
        case 0xB0: gxFmt = 0x0E; break;
        case 0xA0: gxFmt = 0x01; break;
        default:   gxFmt = -1;   break;
    }

    /* Step 5: Set up GXTlutObj if TLUT data exists */
    if (tex->tlutOffset != 0) {
        u32 palSize;
        u32 tlutWrap;

        palSize = 0;
        switch (tex->format) {
            case 0x00: palSize = 0x10;  break;
            case 0x01: palSize = 0x100; break;
            case 0x30: palSize = 0x400; break;
            default:   break;
        }

        tlutWrap = 0;
        switch (tex->tlutFormat) {
            case 1: tlutWrap = 0; break;
            case 2: tlutWrap = 1; break;
            case 3: tlutWrap = 2; break;
            default: break;
        }

        fn_800BB050(tex->gxTlutObj, (void*)tex->tlutOffset, tlutWrap);
    }

    /* Step 6: Set up GXTexObj */
    hasMips = (tex->mipLevels == 1) ? 0 : 1;
    fn_800BA9E4(tex->gxTexObj, tex->data, tex->width, tex->height,
                gxFmt, 0, 0, hasMips);

    /* Step 7: Mark dirty, clear memHandle */
    tex->dirty = 1;
    tex->memHandle = 0;

    return tex;
}

/*
 * GSthread context block layout (pointed to by lbl_8047AC1C):
 *   +0x00 : r0           (u32)
 *   +0x04 : (padding / r1 saved separately via stack)
 *   +0x08 : r2
 *   +0x0C : r3
 *   +0x10 : r4
 *   ...   : r5-r31 at +0x14 .. +0x7C
 *   +0x80 : saved LR
 *   +0x84 : saved CTR / scratch
 *   +0x88 : f0 (f64, 8 bytes each)
 *   ...   : f1-f31 at +0x90 .. +0x180
 *
 * typedef matching this layout:
 */
typedef struct GSThreadCtx {
    u32  r0;          /* +0x00 */
    u32  sp;          /* +0x04  saved stack pointer (r1) */
    u32  r2;          /* +0x08 */
    u32  r3;          /* +0x0C */
    u32  r4;          /* +0x10 */
    u32  r5;          /* +0x14 */
    u32  r6;          /* +0x18 */
    u32  r7;          /* +0x1C */
    u32  r8;          /* +0x20 */
    u32  r9;          /* +0x24 */
    u32  r10;         /* +0x28 */
    u32  r11;         /* +0x2C */
    u32  r12;         /* +0x30 */
    u32  r13;         /* +0x34 */
    u32  r14;         /* +0x38 */
    u32  r15;         /* +0x3C */
    u32  r16;         /* +0x40 */
    u32  r17;         /* +0x44 */
    u32  r18;         /* +0x48 */
    u32  r19;         /* +0x4C */
    u32  r20;         /* +0x50 */
    u32  r21;         /* +0x54 */
    u32  r22;         /* +0x58 */
    u32  r23;         /* +0x5C */
    u32  r24;         /* +0x60 */
    u32  r25;         /* +0x64 */
    u32  r26;         /* +0x68 */
    u32  r27;         /* +0x6C */
    u32  r28;         /* +0x70 */
    u32  r29;         /* +0x74 */
    u32  r30;         /* +0x78 */
    u32  r31;         /* +0x7C */
    u32  lr;          /* +0x80  saved link register */
    u32  ctr;         /* +0x84  saved return-fn pointer */
    f64  f[32];       /* +0x88  f0-f31 (8 bytes each = 0x100 bytes) */
} GSThreadCtx;

/* 0x800F028C | 0x68 */
extern void threadSaveGPRRegisters(void);
extern void threadLoadGPRRegisters(void);
extern void threadLoadFPRRegisters(void);
extern u32 lbl_8047AC24;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC10;
#if 1
asm void threadExecute(void) {
#include "src/game/gs_texture_fn_800F028C.inc"
}
#else
/*
 * threadExecute -- cooperative thread yield.
 *
 * Switches from the "next" thread context (lbl_8047AC24) to the
 * "idle/run" context (lbl_8047AC20), saving the current call frame
 * so that fn_800F02F4 can restore it on re-entry.
 *
 * Assembly flow:
 *   1. Allocate a 0xC-byte frame; save r3 (arg) and LR.
 *   2. lbl_8047AC1C = lbl_8047AC24  (switch to "next" ctx)
 *   3. call threadSaveGPRRegisters (save all GPRs into ctx)
 *   4. store r1 (stack ptr) into ctx->sp
 *   5. lbl_8047AC1C = lbl_8047AC20  (switch to "run" ctx)
 *   6. call threadLoadGPRRegisters (restore GPRs from new ctx)
 *   7. if lbl_8047AC10 != 0: call threadLoadFPRRegisters (restore FPRs)
 *   8. Restore LR from ctx->lr, pop frame, branch via ctx->ctr.
 *
 * This function cannot be matched in C because it manipulates the
 * stack pointer and all GPRs/FPRs directly.
 */
void threadExecute(void) {
    GSThreadCtx* ctx;

    lbl_8047AC1C = lbl_8047AC24;
    threadSaveGPRRegisters();
    ctx = (GSThreadCtx*)lbl_8047AC1C;
    ctx->sp = (u32)0; /* r1 stored into ctx->sp in asm */

    lbl_8047AC1C = lbl_8047AC20;
    threadLoadGPRRegisters();

    if (lbl_8047AC10 != 0) {
        threadLoadFPRRegisters();
    }
}
#endif

/* 0x800F02F4 | 0x14 */
#if 1
asm void fn_800F02F4(void) {
#include "src/game/gs_texture_fn_800F02F4.inc"
}
#else
/*
 * fn_800F02F4 -- cooperative thread yield return epilogue.
 *
 * Restores the frame that threadExecute allocated (0xC bytes) and
 * returns to the caller's LR.  Called as a tail from threadExecute
 * via the ctx->lr / ctx->ctr fields.
 *
 * Assembly:
 *   lwz r3, 0x8(r1)   ; saved LR
 *   mtlr r3
 *   lwz r3, 0xC(r1)   ; original r3 (arg)
 *   addi r1, r1, 0xC  ; pop frame
 *   blr
 *
 * Cannot be matched in C -- it pops a frame and restores LR manually.
 */
void fn_800F02F4(void) {
}
#endif

/* 0x800F036C | 0x8 */
#if 0
asm void fn_800F036C(void) {
#include "src/game/gs_texture_fn_800F036C.inc"
}
#else
/* lbz r3, 0xb(r3) -- load byte at offset 0xb from arg */
u8 fn_800F036C(GSThread* thr) {
    return thr->affinity;
}
#endif

/* 0x800F0374 | 0x8 */
#if 0
asm void fn_800F0374(void) {
#include "src/game/gs_texture_fn_800F0374.inc"
}
#else
/* lwz r3, 0xc(r3) -- load word at offset 0xc from arg */
u32 fn_800F0374(GSThread* thr) {
    return thr->priority;
}
#endif

/* 0x800F037C | 0x8 */
extern u32 lbl_8047AC00;
#if 0
asm void fn_800F037C(void) {
#include "src/game/gs_texture_fn_800F037C.inc"
}
#else
/* lwz r3, lbl_8047AC00(r13) -- return gsThreadFrameCount */
u32 fn_800F037C(void) {
    return lbl_8047AC00;
}
#endif

/* 0x800F0384 | 0x50 */
extern u32 lbl_8047AC28;
extern u32 lbl_8047AC30;
extern u32 lbl_8047AC0C;
#if 0
asm void fn_800F0384(void) {
#include "src/game/gs_texture_fn_800F0384.inc"
}
#else
/*
 * Loop over all threads; for each thread whose priority == arg, clear sleeping flag.
 * stride = 0x24, field at +0xc = priority, field at +0xa = sleeping
 * lbl_8047AC28 = gsThreadArray, lbl_8047AC30 = gsThreadMaxCount
 * lbl_8047AC0C = gsThreadActive
 */
void fn_800F0384(u32 priority) {
    u32 i;
    u32 offset;
    GSThread* arr;
    u32 count;
    GSThread* thr;

    if (priority == 0) {
        return;
    }

    arr = (GSThread*)lbl_8047AC28;
    count = lbl_8047AC30;
    offset = 0;
    i = 0;

    while (i < count) {
        thr = (GSThread*)((u8*)arr + offset);
        if (thr->priority == priority) {
            thr->sleeping = 0;
        }
        offset += 0x24;
        i++;
    }

    *(u8*)&lbl_8047AC0C = 1;
}
#endif

/* 0x800F03D4 | 0x50 */
extern u32 lbl_8047AC28;
extern u32 lbl_8047AC30;
extern u32 lbl_8047AC0C;
#if 0
asm void fn_800F03D4(void) {
#include "src/game/gs_texture_fn_800F03D4.inc"
}
#else
/*
 * Loop over all threads; for each thread whose priority == arg, set sleeping = 1.
 */
void fn_800F03D4(u32 priority) {
    u32 i;
    u32 offset;
    GSThread* arr;
    u32 count;
    GSThread* thr;

    if (priority == 0) {
        return;
    }

    arr = (GSThread*)lbl_8047AC28;
    count = lbl_8047AC30;
    offset = 0;
    i = 0;

    while (i < count) {
        thr = (GSThread*)((u8*)arr + offset);
        if (thr->priority == priority) {
            thr->sleeping = 1;
        }
        offset += 0x24;
        i++;
    }

    *(u8*)&lbl_8047AC0C = 1;
}
#endif
extern u32 lbl_8047AC1C;
#if 1
asm void threadLoadGPRRegisters(void) {
#include "src/game/gs_texture_fn_800F0030.inc"
}
#else
/*
 * threadLoadGPRRegisters -- Restore all GPRs from the current context block.
 *
 * Loads lbl_8047AC1C (GSThreadCtx*) and restores:
 *   r0  from ctx+0x00
 *   r2  from ctx+0x08
 *   r4-r31 from ctx+0x10 .. ctx+0x7C
 * Then restores original r3 from the caller's stack slot (0x8(r1))
 * and pops the 0x8-byte frame before returning.
 *
 * Cannot be matched in C -- touches all GPRs directly.
 */
void threadLoadGPRRegisters(void) {
    GSThreadCtx* ctx = (GSThreadCtx*)lbl_8047AC1C;
    (void)ctx->r0;
    (void)ctx->r2;
    (void)ctx->r4;
    /* ... r5-r31 ... */
    (void)ctx->r31;
}
#endif
extern u32 lbl_8047AC1C;
#if 1
asm void threadLoadFPRRegisters(void) {
#include "src/game/gs_texture_fn_800F00C0.inc"
}
#else
/*
 * threadLoadFPRRegisters -- Restore all FPRs from the current context block.
 *
 * Loads lbl_8047AC1C (GSThreadCtx*), adds 0x88 to get to the FPR
 * save area, then loads f0-f31 (lfd, 8 bytes each) from offsets
 * +0x0 through +0xF8.
 *
 * Cannot be matched in C -- touches all FPRs directly.
 */
void threadLoadFPRRegisters(void) {
    GSThreadCtx* ctx = (GSThreadCtx*)lbl_8047AC1C;
    (void)ctx->f[0];
    /* ... f1-f31 ... */
    (void)ctx->f[31];
}
#endif
extern u32 lbl_8047AC1C;
#if 1
asm void threadSaveGPRRegisters(void) {
#include "src/game/gs_texture_fn_800F015C.inc"
}
#else
/*
 * threadSaveGPRRegisters -- Save all GPRs into the current context block.
 *
 * Loads lbl_8047AC1C (GSThreadCtx*) and stores:
 *   r0  to ctx+0x00
 *   r2  to ctx+0x08
 *   r3  (self ptr) to ctx+0x0C
 *   r4-r31 to ctx+0x10 .. ctx+0x7C
 * Then restores r3 from caller stack slot and pops 0x8-byte frame.
 *
 * Returns the ctx pointer in r3 (callers use ctx->sp, ctx->lr etc.)
 *
 * Cannot be matched in C -- touches all GPRs directly.
 */
void threadSaveGPRRegisters(void) {
    GSThreadCtx* ctx = (GSThreadCtx*)lbl_8047AC1C;
    ctx->r0 = 0;
    ctx->r2 = 0;
    ctx->r3 = (u32)ctx;
    ctx->r4 = 0;
    /* ... r5-r31 ... */
    ctx->r31 = 0;
}
#endif
extern u32 lbl_8047AC1C;
#if 0
asm void fn_800F01F0(void) {
#include "src/game/gs_texture_fn_800F01F0.inc"
}
#else
/*
 * fn_800F01F0 -- Save all FPRs into the current context block.
 *
 * Loads lbl_8047AC1C (GSThreadCtx*), adds 0x88 to get to the FPR
 * save area, then stores f0-f31 (stfd, 8 bytes each) to offsets
 * +0x0 through +0xF8.
 */
void fn_800F01F0(u32 arg) {
    GSThreadCtx* ctx = (GSThreadCtx*)lbl_8047AC1C;
    f64 src = *(f64*)&arg;
    ctx->f[0]  = src;
    ctx->f[1]  = src;
    ctx->f[2]  = src;
    ctx->f[3]  = src;
    ctx->f[4]  = src;
    ctx->f[5]  = src;
    ctx->f[6]  = src;
    ctx->f[7]  = src;
    ctx->f[8]  = src;
    ctx->f[9]  = src;
    ctx->f[10] = src;
    ctx->f[11] = src;
    ctx->f[12] = src;
    ctx->f[13] = src;
    ctx->f[14] = src;
    ctx->f[15] = src;
    ctx->f[16] = src;
    ctx->f[17] = src;
    ctx->f[18] = src;
    ctx->f[19] = src;
    ctx->f[20] = src;
    ctx->f[21] = src;
    ctx->f[22] = src;
    ctx->f[23] = src;
    ctx->f[24] = src;
    ctx->f[25] = src;
    ctx->f[26] = src;
    ctx->f[27] = src;
    ctx->f[28] = src;
    ctx->f[29] = src;
    ctx->f[30] = src;
    ctx->f[31] = src;
}
#endif
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC24;
#if 1
asm void _threadSwitch(void) {
#include "src/game/gs_texture_fn_800F0308.inc"
}
#else
/*
 * _threadSwitch -- cooperative thread "call" context switch.
 *
 * Suspends the current thread (lbl_8047AC20 = "run" ctx) and
 * dispatches to a new thread via the "next" context (lbl_8047AC24).
 * Saves the caller's function pointer (r3) and stack frame (r1)
 * into the context block so that threadExecute can resume it later.
 *
 * Assembly flow:
 *   1. Save r3 (func ptr) to lbl_8047AC18, r5 (arg) to lbl_8047AC14.
 *   2. Save LR to r5.
 *   3. lbl_8047AC1C = lbl_8047AC20  (switch to "run" ctx)
 *   4. call threadSaveGPRRegisters (save all GPRs into run-ctx)
 *   5. ctx->lr  = saved LR (r5)
 *   6. ctx->r3  = lbl_8047AC18 (func ptr)
 *   7. ctx->r5  = lbl_8047AC14 (arg)
 *   8. ctx->sp  = r1 (stack ptr)
 *   9. if lbl_8047AC10 != 0: call fn_800F01F0 (save FPRs)
 *  10. lbl_8047AC1C = lbl_8047AC24  (switch to "next" ctx)
 *  11. call threadLoadGPRRegisters (restore GPRs from next-ctx)
 *  12. Restore LR from ctx->lr, r5 from ctx->r5, r3 from ctx->r3, blr.
 *
 * Cannot be matched in C -- manipulates stack and all registers.
 */
void _threadSwitch(void) {
    GSThreadCtx* ctx;
    u32 lr;

    lbl_8047AC18 = (u32)0; /* r3 (func ptr) */
    lbl_8047AC14 = (u32)0; /* r5 (arg) */

    lbl_8047AC1C = lbl_8047AC20;
    threadSaveGPRRegisters();
    ctx = (GSThreadCtx*)lbl_8047AC1C;
    ctx->lr  = lr;
    ctx->r3  = lbl_8047AC18;
    ctx->r5  = lbl_8047AC14;
    ctx->sp  = (u32)0; /* r1 */

    if (lbl_8047AC10 != 0) {
        fn_800F01F0();
    }

    lbl_8047AC1C = lbl_8047AC24;
    threadLoadGPRRegisters();
}
#endif
extern u32 lbl_8047AC0C;
#if 0
asm void fn_800F0424(void) {
#include "src/game/gs_texture_fn_800F0424.inc"
}
#else
void fn_800F0424(GSThread* thr) {
    thr->suspended = 0;
    *(u8*)&lbl_8047AC0C = 1;
}
#endif
void fn_800F0438(u8* p) {
    p[0x9] = 1;
    *(u8*)&lbl_8047AC0C = 1;
}
extern void fn_800F0A74(u32 priority, u32 flag); /* thread wake/sleep dispatch */
/* 0x800F0448 | 0x18 */
void fn_800F0448(void) {
    fn_800F0A74(0, 0);
}
/* 0x800F0470 | 0x18 */
void fn_800F0470(u32 priority) {
    fn_800F0A74(priority, 1);
}
/* 0x800F0494 | 0x24 */
u32 fn_800F0494(GSThread* thr) {
    u32 ret;
    if (thr->autoStart == 1) {
        return 0;
    }
    ret = thr->unused;
    thr->active = 0;
    return ret;
}
u8 fn_800F04BC(u8* p) { return p[0x14]; }
extern u32 lbl_8047AC00;
extern u32 lbl_8047AC04;
extern u32 lbl_8047AC08;
extern u32 lbl_8047AC0C;
extern void fn_800E209C(u16 handle); /* GSmemFree */
/* 0x800F04C4 | 0xB4 */
/*
 * fn_800F04C4 -- destroy every thread whose priority == @p priority.
 *
 * Walks the global thread list (head lbl_8047AC08); for each matching
 * thread it applies the same destroy/unlink logic as fn_800F05A0
 * (inlined): current/run threads are deferred via the 0x15 pending flag
 * (+ lbl_8047AC0C reschedule for the run thread); all others are
 * unlinked from the list and have their stack/ctx GSmem handles freed.
 */
void fn_800F04C4(u32 priority) {
    GSThread* thr;
    for (thr = (GSThread*)lbl_8047AC08; thr != NULL; thr = thr->next) {
        if (thr->priority == priority) {
            if (thr == (GSThread*)lbl_8047AC00 || thr == (GSThread*)lbl_8047AC04) {
                thr->pad1 = 1;
                if (thr == (GSThread*)lbl_8047AC04) {
                    *(u8*)&lbl_8047AC0C = 1;
                }
            } else {
                thr->pad0 = 0;
                thr->active = 0;
                if (thr->prev != NULL) {
                    thr->prev->next = thr->next;
                }
                if (thr->next != NULL) {
                    thr->next->prev = thr->prev;
                }
                if ((GSThread*)lbl_8047AC08 == thr) {
                    lbl_8047AC08 = (u32)thr->next;
                }
                fn_800E209C(thr->stackHandle);
                fn_800E209C(thr->ctxHandle);
            }
        }
    }
}
/* 0x800F05A0 | 0xA8 */
/*
 * fn_800F05A0 -- destroy / unregister a cooperative thread (GSThread).
 *
 * If @p thr is the current thread (lbl_8047AC00) or the run thread
 * (lbl_8047AC04) it cannot be torn down in place, so a destroy-pending
 * flag at offset 0x15 is raised instead; when it is the run thread the
 * global reschedule flag (lbl_8047AC0C) is set so the dispatcher re-enters.
 *
 * Otherwise the thread is unlinked from the doubly-linked thread list,
 * the list head (lbl_8047AC08) is fixed up if it pointed at this thread,
 * and both the stack and context GSmem handles are released via
 * fn_800E209C (GSmemFree).
 */
void fn_800F05A0(GSThread* thr) {
    if (thr == (GSThread*)lbl_8047AC00 || thr == (GSThread*)lbl_8047AC04) {
        thr->pad1 = 1;
        if (thr == (GSThread*)lbl_8047AC04) {
            *(u8*)&lbl_8047AC0C = 1;
        }
    } else {
        thr->pad0 = 0;
        thr->active = 0;
        if (thr->prev != NULL) {
            thr->prev->next = thr->next;
        }
        if (thr->next != NULL) {
            thr->next->prev = thr->prev;
        }
        if ((GSThread*)lbl_8047AC08 == thr) {
            lbl_8047AC08 = (u32)thr->next;
        }
        fn_800E209C(thr->stackHandle);
        fn_800E209C(thr->ctxHandle);
    }
}
void fn_800F0654(void) { /* TODO */ }
