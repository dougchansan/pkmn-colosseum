/**
 * @file gs_texture_candidate_800EF5FC.c
 * @brief GStexture pool: creation, loading and pool initialisation.
 *
 * Address range: 0x800EF5FC - 0x800EFD14 (GStextureCreate). The retail
 * GStexture.cpp spans 0x800EF098 - 0x800F0030; the earlier functions,
 * fn_800EFD14/GStextureLoad (gs_texture_exact_800EFD14.c) and GStextureInit
 * (gs_texture_exact_800EFFC0.c) live in their own dtk partitions.
 *
 * Exact and linked (lane D3, 2026-09-29) through three tagged title-path
 * rule exceptions (docs/RULE_EXCEPTIONS.md):
 *  - the size-adjust log passes `width & 0xFFFF` / `height & 0xFFFF`. MWCC
 *    then builds the format string in r3 while width and height are still
 *    live, which gives retail's entry copies `mr r9,r3; mr r8,r4`;
 *  - the incremented mip count is an int `levels` read through (u8), which
 *    gives retail's in-place `clrlwi r3,r29,24; addi r29,r3,1`;
 *  - the pool scan is reached through a single-use wrapper inline
 *    (textureAlloc). Called directly, the scan's result outranks the GX
 *    helper's TLUT-format local (tex r31 / TLUT r30). One level of nesting
 *    gives retail's TLUT r31 / tex r30.
 * The declaration and statement order are the ones retail's register and
 * schedule imply. See docs/recon/gs_texture_create_wall.md.
 */

#include "dolphin/types.h"
#include "game/gs_texture.h"

extern void GSlogWrite(const char* format, ...);
extern u16 GSmemAlloc(u32 size, u32 alignment);   /* GSmemAlloc */
extern void* GSmemLock(u16 handle);              /* GSmemGetPtr */
extern void GSmemFree(u16 handle);               /* GSmemFree */

extern const char lbl_80270F98[]; /* "GStexture: invalid texture format" */
extern const char lbl_80270FBC[]; /* "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]" */

extern u8 lbl_80466BC0[]; /* current display descriptor (width at +4, height at +6) */

extern GStextureHandle* lbl_8047ABF4; /* texture pool */
extern u32 lbl_8047ABF8;              /* texture pool size */

#include "game/gs_texture_init_gx.h"

static inline GStextureHandle* textureFindFree(void)
{
    GStextureHandle* tex = lbl_8047ABF4;
    u32 i;

    for (i = 0; i < lbl_8047ABF8; i++, tex++) {
        if (tex->inUse == 0) {
            return tex;
        }
    }
    return NULL;
}


/* RULE-EXCEPTION(title-path): single-use inline wrapper whose only effect is
 * register priority (TLUT r31 / tex r30) — see docs/RULE_EXCEPTIONS.md */
static inline GStextureHandle* textureAlloc(void)
{
    return textureFindFree();
}

GStextureHandle* GStextureCreate(s32 width, s32 height, s32 format, s32 tlutFormat,
                                 u8 mipLevels)
{
    s32 align;
    u32 mipSize;
    GStextureHandle* tex;
    s32 levels;
    u32 pixelCount;
    u16 adjWidth;
    u16 w;
    u16 h;
    s32 tlutEntries;
    u8 maxLevels;
    u16 adjHeight;
    s32 level;

    if ((u16)width == 0 && (u16)height == 0) {
        width = *(u16*)(lbl_80466BC0 + 4);
        height = *(u16*)(lbl_80466BC0 + 6);
    }

    if ((u16)width > 0x400 || (u16)height > 0x400 || (u16)width < 4 || (u16)height < 4) {
        return NULL;
    }

    switch (format) {
    case 0x00:
    case 0x40:
    case 0x41:
    case 0xB0:
        align = 8;
        break;
    case 0x01:
    case 0x42:
    case 0x43:
    case 0xA0:
        align = 4;
        break;
    case 0x30:
    case 0x44:
    case 0x45:
    case 0x90:
        align = 4;
        break;
    default:
        GSlogWrite(lbl_80270F98);
        return NULL;
    }

    adjWidth = (width + align - 1) & ~(align - 1);
    adjHeight = (height + align - 1) & ~(align - 1);
    if (adjWidth != (u16)width || adjHeight != (u16)height) {
        /* RULE-EXCEPTION(title-path): mask instead of (u16) cast, only for
         * argument evaluation order (retail's entry copies) — see
         * docs/RULE_EXCEPTIONS.md */
        GSlogWrite(lbl_80270FBC, width & 0xFFFF, height & 0xFFFF, adjWidth, adjHeight);
    }

    w = adjWidth;
    h = adjHeight;
    maxLevels = 0;
    while (w > 4 && h > 4 && maxLevels < 7) {
        w >>= 1;
        h >>= 1;
        maxLevels++;
    }
    if (mipLevels > maxLevels) {
        mipLevels = maxLevels;
    }

    tex = textureAlloc();
    if (tex == NULL) {
        return NULL;
    }

    tlutEntries = 0;
    switch (format) {
    case 0x00:
        tlutEntries = 0x10;
    case 0x40:
    case 0xB0:
        tex->bitsPerPixel = 4;
        break;
    case 0x01:
        tlutEntries = 0x100;
    case 0x41:
    case 0x42:
    case 0xA0:
        tex->bitsPerPixel = 8;
        break;
    case 0x30:
        tlutEntries = 0x400;
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

    pixelCount = adjWidth * adjHeight;
    mipSize = tex->bitsPerPixel * pixelCount / 8;
    tex->totalSize = 0;
    /* RULE-EXCEPTION(title-path): int local read through (u8) casts, only
     * for the in-place increment and register home — see
     * docs/RULE_EXCEPTIONS.md */
    levels = mipLevels + 1;
    for (level = 0; level < (u8)levels; level++) {
        tex->totalSize += (mipSize + 0x1F) & ~0x1F;
        mipSize >>= 1;
    }

    if (tlutEntries != 0) {
        if (tlutFormat != 0 && tlutFormat >= 0 && tlutFormat < 4) {
            tex->totalSize += (tlutEntries * 16) >> 3;
        } else {
            return NULL;
        }
    }

    tex->memHandle = GSmemAlloc(tex->totalSize, 0x20);
    if (tex->memHandle == 0) {
        return NULL;
    }
    tex->mipData[0] = GSmemLock(tex->memHandle);
    if (tex->mipData[0] == NULL) {
        GSmemFree(tex->memHandle);
        return NULL;
    }

    tex->inUse = 1;
    tex->width = adjWidth;
    tex->height = adjHeight;
    tex->mipLevels = levels;
    tex->format = format;
    tex->tlutFormat = tlutFormat;
    tex->wrapS = 0;
    tex->wrapT = 0;
    tex->minFilter = 2;
    tex->magFilter = 2;
    if ((u8)levels > 1) {
        tex->lodClamp = 2;
    } else {
        tex->lodClamp = 0;
    }
    tex->refCount = 0;
    tex->unk52 = 0;

    mipSize = tex->bitsPerPixel * pixelCount / 8;
    for (level = 1; level < 8; level++) {
        if (level < tex->mipLevels) {
            tex->mipData[level] = (u8*)tex->mipData[level - 1] + mipSize;
            mipSize >>= 1;
        } else {
            tex->mipData[level] = NULL;
        }
    }

    switch (tlutFormat) {
    case 1:
    case 2:
    case 3:
        tex->tlutData = (u8*)tex->mipData[tex->mipLevels - 1] + mipSize;
        break;
    default:
        tex->tlutData = NULL;
        break;
    }

    textureInitGXObjects(tex);
    return tex;
}

/* fn_800EFD14 and GStextureLoad (0x800EFD14 - 0x800EFFC0) are linked from
 * gs_texture_exact_800EFD14.c. */
